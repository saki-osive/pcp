#
# Copyright (c) 2009 Josef 'Jeff' Sipek <jeffpc@josefsipek.net>
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
#

use strict;
use warnings;
use PCP::PMDA;
use Time::HiRes qw ( time );

use vars qw( $pmda );
use vars qw( %caches );
my $qshape = 'qshape -b 10 -t 5';
my $refresh = 5.0; # 5 seconds between refreshes

my $cached = 0;

sub postfix_do_refresh
{
    QUEUE:
    foreach my $qname ("maildrop", "incoming", "hold", "active", "deferred") {
	unless (open(PIPE, "$qshape $qname |")) {
	    $pmda->log("couldn't execute '$qshape $qname'");
	     next QUEUE;
	}
	while(<PIPE>) {
	    last if (/^[\t ]*TOTAL /);
	}
	close PIPE;

	unless (/^[\t ]*TOTAL /) {
	    $pmda->log("malformed output for '$qshape $qname': $_");
	    next QUEUE;
	}

	s/^[\t ]*//;
	s/[\t ]+/ /g;

	my @items = split(/ /);

	$caches{$qname}{0}  = $items[1];
	$caches{$qname}{1}  = $items[2];
	$caches{$qname}{2}  = $items[3];
	$caches{$qname}{3}  = $items[4];
	$caches{$qname}{4}  = $items[5];
	$caches{$qname}{5}  = $items[6];
	$caches{$qname}{6}  = $items[7];
	$caches{$qname}{7}  = $items[8];
	$caches{$qname}{8}  = $items[9];
	$caches{$qname}{9}  = $items[10];
	$caches{$qname}{10} = $items[11];
    }
}

sub postfix_fetch_callback
{
    my ($cluster, $item, $inst) = @_;
    my $metric_name = pmda_pmid_name($cluster, $item);

    my $now = time;

    #$pmda->log("postfix_fetch_callback $metric_name $cluster:$item ($inst)\n");

    if (!defined($metric_name))    { return (PM_ERR_PMID, 0); }

    if ($now - $cached > $refresh) {
	postfix_do_refresh();
	$cached = $now;
    }

    if ($cluster == 0) {
	my $qname;

	$qname = $metric_name;
	$qname =~ s/^postfix\.queues\.//;

	return (PM_ERR_AGAIN, 0) unless defined($caches{$qname});
	return ($caches{$qname}{$inst}, 1);
    }

    return (PM_ERR_PMID, 0);
}

my $postfix_queues_indom = 0;
my @postfix_queues_dom = (    0 => 'total',
		1 => '0-5 mins',
		2 => '5-10 mins',
		3 => '10-20 mins',
		4 => '20-40 mins',
		5 => '40-80 mins',
		6 => '80-160 mins',
		7 => '160-320 mins',
		8 => '320-640 mins',
		9 => '640-1280 <callto:+1640-1280> mins',
		10=> '1280+ mins',
	     );

$pmda = PCP::PMDA->new('postfix', 103);

$pmda->add_metric(pmda_pmid(0,0), PM_TYPE_U32, $postfix_queues_indom,
	PM_SEM_INSTANT, pmda_units(0,0,1,0,0,PM_COUNT_ONE),
	"postfix.queues.maildrop", '', '');
$pmda->add_metric(pmda_pmid(0,1), PM_TYPE_U32, $postfix_queues_indom,
	PM_SEM_INSTANT, pmda_units(0,0,1,0,0,PM_COUNT_ONE),
	"postfix.queues.incoming", '', '');
$pmda->add_metric(pmda_pmid(0,2), PM_TYPE_U32, $postfix_queues_indom,
	PM_SEM_INSTANT, pmda_units(0,0,1,0,0,PM_COUNT_ONE),
	"postfix.queues.hold", '', '');
$pmda->add_metric(pmda_pmid(0,3), PM_TYPE_U32, $postfix_queues_indom,
	PM_SEM_INSTANT, pmda_units(0,0,1,0,0,PM_COUNT_ONE),
	"postfix.queues.active", '', '');
$pmda->add_metric(pmda_pmid(0,4), PM_TYPE_U32, $postfix_queues_indom,
	PM_SEM_INSTANT, pmda_units(0,0,1,0,0,PM_COUNT_ONE),
	"postfix.queues.deferred", '', '');

$pmda->add_indom($postfix_queues_indom, \@postfix_queues_dom, '', '');
$pmda->set_fetch_callback(\&postfix_fetch_callback);
$pmda->run;

=pod

=head1 NAME

pmdapostfix - Postfix performance metrics domain agent (PMDA)

=head1 DESCRIPTION

B<pmdapostfix> is a Performance Metrics Domain Agent (PMDA) which exports
mail queue sizes as reported by qshape(1).

=head1 INSTALLATION

If you want access to the names and values for the Postfix performance
metrics, do the following as root:

    # cd $PCP_PMDAS_DIR/postfix
    # ./Install

If you want to undo the installation, do the following as root:

    # cd $PCP_PMDAS_DIR/postfix
    # ./Remove

B<pmdapostfix> is launched by pmcd(1) and should never be executed
directly.  The Install and Remove scripts notify pmcd(1) when
the agent is installed or removed.

=head1 FILES

=over

=item $PCP_PMDAS_DIR/postfix/Install

installation script for the B<pmdapostfix> agent

=item $PCP_PMDAS_DIR/postfix/Remove

undo installation script for the B<pmdapostfix> agent

=item $PCP_LOG_DIR/pmcd/postfix.log

default log file for error messages from B<pmdapostfix>

=back

=head1 SEE ALSO

pmcd(1) and qshape(1).
