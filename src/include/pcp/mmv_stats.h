/*
 * Copyright (C) 2013,2016 Red Hat.
 * Copyright (C) 2009 Aconex.  All Rights Reserved.
 * Copyright (C) 2001,2009 Silicon Graphics, Inc.  All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 */
#ifndef PCP_MMV_STATS_H
#define PCP_MMV_STATS_H

//#include "mmv_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MMV_NAMEMAX	64
#define MMV_STRINGMAX	256
#define MMV_LABELMAX    232

typedef enum mmv_metric_type {
    MMV_TYPE_NOSUPPORT = PM_TYPE_NOSUPPORT,
    MMV_TYPE_I32       = PM_TYPE_32,	/* 32-bit signed integer */
    MMV_TYPE_U32       = PM_TYPE_U32,	/* 32-bit unsigned integer */
    MMV_TYPE_I64       = PM_TYPE_64,	/* 64-bit signed integer */
    MMV_TYPE_U64       = PM_TYPE_U64,	/* 64-bit unsigned integer */
    MMV_TYPE_FLOAT     = PM_TYPE_FLOAT,	/* 32-bit floating point */
    MMV_TYPE_DOUBLE    = PM_TYPE_DOUBLE,/* 64-bit floating point */
    MMV_TYPE_STRING    = PM_TYPE_STRING,/* NULL-terminate string */
    MMV_TYPE_ELAPSED   = 9,		/* 64-bit elapsed time */
} mmv_metric_type_t;

typedef enum mmv_metric_sem {
    MMV_SEM_COUNTER	= PM_SEM_COUNTER,
    MMV_SEM_INSTANT	= PM_SEM_INSTANT,
    MMV_SEM_DISCRETE	= PM_SEM_DISCRETE,
} mmv_metric_sem_t;

typedef struct mmv_instances {
    __int32_t		internal;	/* Internal instance ID */
    char		external[MMV_NAMEMAX];	/* External instance ID */
} mmv_instances_t;

typedef struct mmv_instances2 {
    __int32_t		internal;	/* Internal instance ID */
    char *		external;	/* External instance ID */
} mmv_instances2_t;

typedef struct mmv_indom {
    __uint32_t		serial;		/* Unique identifier */
    __uint32_t		count;		/* Number of instances */
    mmv_instances_t *	instances;	/* Internal/external IDs */
    char *		shorttext;	/* Short help text string */
    char *		helptext;	/* Long help text string */
} mmv_indom_t;

typedef struct mmv_indom2 {
    __uint32_t		serial;		/* Unique identifier */
    __uint32_t		count;		/* Number of instances */
    mmv_instances2_t *	instances;	/* Internal/external IDs */
    char *		shorttext;	/* Short help text string */
    char *		helptext;	/* Long help text string */
} mmv_indom2_t;

typedef struct mmv_metric {
    char		name[MMV_NAMEMAX];
    __uint32_t		item;		/* Unique identifier */
    mmv_metric_type_t	type;
    mmv_metric_sem_t	semantics;
    pmUnits		dimension;
    __uint32_t		indom;		/* Indom serial */
    char *		shorttext;	/* Short help text string */
    char *		helptext;	/* Long help text string */
} mmv_metric_t;

typedef struct mmv_metric2 {
    char *		name;
    __uint32_t		item;		/* Unique identifier */
    mmv_metric_type_t	type;
    mmv_metric_sem_t	semantics;
    pmUnits		dimension;
    __uint32_t		indom;		/* Indom serial */
    char *		shorttext;	/* Short help text string */
    char *		helptext;	/* Long help text string */
} mmv_metric2_t;

typedef struct mmv_label {
    __uint32_t        flags;      /* PM_LABEL_TYPE, flag optional */
    __uint32_t        identity;   /* Indom, Cluster or item ID */
    __int32_t         internal;   /* Instance ID or PM_IN_NULL */
    __uint32_t        name;       /* Length of the label name */
    __uint32_t        value;      /* Length of the label value*/
    char *            payload[MMV_LABELMAX];
} mmv_label_t;

typedef enum mmv_stats_flags {
    MMV_FLAG_NOPREFIX	= 0x1,	/* Don't prefix metric names by filename */
    MMV_FLAG_PROCESS	= 0x2,	/* Indicates process check on PID needed */
    MMV_FLAG_SENTINEL	= 0x4,	/* Sentinel values == no-value-available */
} mmv_stats_flags_t;

typedef struct mmv_registry {
    mmv_indom2_t *      indoms;
    __uint32_t          nindoms;
    mmv_metric2_t *     metrics;
    __uint32_t          nmetrics;
    mmv_instances2_t *  instances;
    __uint32_t          ninstances;
    mmv_label_t *       labels;
    __uint32_t          nlabels;
    __uint32_t          version;
    char *              file;
    __uint32_t          cluster;
    mmv_stats_flags_t   flags;
} mmv_registry_t;

#ifdef HAVE_BITFIELDS_LTOR
#define MMV_UNITS(a,b,c,d,e,f)	{a,b,c,d,e,f,0}
#else
#define MMV_UNITS(a,b,c,d,e,f)	{0,f,e,d,c,b,a}
#endif

//typedef enum mmv_stats_flags {
//     MMV_FLAG_SENTINEL	= 0x4,	/* Sentinel values == no-value-available */
// } mmv_stats_flags_t;
 
extern mmv_registry_t * mmv_stats_registry(const char *file, int cluster, mmv_stats_flags_t flags);
//extern int mmv_stats_add_indom(void *registry,
//                                int serial, const char *shorthelp, const char *longhelp);
//extern void * mmv_stats_add_indom_label(void *registry);	/* PM_LABEL_CLUSTER */
extern int mmv_stats_add_metric(void *registry, const char *name, int item,
                                mmv_metric_type_t type, mmv_metric_sem_t sem, pmUnits units,
                                int serial, const char *shorthelp, const char *longhelp);
//extern int mmv_stats_add_instance(void *registry,
//                                int serial, int instid, const char *instname);

//extern void * mmv_stats_add_registry_label(void *registry,	/* PM_LABEL_CLUSTER */
//		const char *name, const char *value);
//extern void * mmv_stats_add_indom_label(void *registry,		/* PM_LABEL_INDOM */
//		int serial, const char *name, const char *value);
//extern void * mmv_stats_add_metric_label(void *registry,	/* PM_LABEL_ITEM */
//		int item, const char *name, const char *value);
//extern void * mmv_stats_add_instance_label(void *registry,	/* PM_LABEL_INSTANCES */
//		int serial, int instid, const char *name, const char *value);

extern void mmv_stats_start(const char *file, void *registry);
extern void mmv_stats_stop(const char *file, void *registry);

/* Deprecated mmv_stats_init routines - do not use */
extern void * mmv_stats_init(const char *, int, mmv_stats_flags_t,
				const mmv_metric_t *, int,
				const mmv_indom_t *, int);
extern void * mmv_stats2_init(const char *, int, mmv_stats_flags_t,
				const mmv_metric2_t *, int,
				const mmv_indom2_t *, int);
extern void mmv_stats_stop(const char *, void *); //deprecated  no anymore necessary/*

extern pmAtomValue * mmv_lookup_value_desc(void *, const char *, const char *);
extern void mmv_inc_value(void *, pmAtomValue *, double);
extern void mmv_set_value(void *, pmAtomValue *, double);
extern void mmv_set_string(void *, pmAtomValue *, const char *, int);

extern void mmv_stats_add(void *, const char *, const char *, double);
extern void mmv_stats_inc(void *, const char *, const char *);
extern void mmv_stats_set(void *, const char *, const char *, double);
extern void mmv_stats_add_fallback(void *, const char *, const char *,
				const char *, double);
extern void mmv_stats_inc_fallback(void *, const char *, const char *,
				const char *);
extern pmAtomValue * mmv_stats_interval_start(void *, pmAtomValue *,
				const char *, const char *);
extern void mmv_stats_interval_end(void *, pmAtomValue *);
extern void mmv_stats_set_string(void *, const char *,
				const char *, const char *);
extern void mmv_stats_set_strlen(void *, const char *,
				const char *, const char *, size_t);

#ifdef __cplusplus
}
#endif

#endif /* PCP_MMV_STATS_H */
