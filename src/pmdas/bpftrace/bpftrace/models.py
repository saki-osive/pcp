from typing import Dict
import uuid
import json
from datetime import datetime


class BPFtraceError(Exception):
    """BPFtrace general error"""


class MetricType:
    """BPFtrace variable types"""
    Control = 'control'
    Histogram = 'histogram'
    Stacks = 'stacks'
    Output = 'output'


class VariableDefinition:
    def __init__(self, single: bool, semantics: int, datatype: int, metrictype: MetricType):
        self.single = single
        self.semantics = semantics
        self.datatype = datatype
        self.metrictype = metrictype


class ScriptMetadata:
    def __init__(self):
        self.name = None
        self.include = None
        self.table_retain_lines = None


class Status:
    Stopped = 'stopped'
    Starting = 'starting'  # starting can take a while
    Started = 'started'
    Stopping = 'stopping'  # if the process doesn't respond to SIGINT, wait 5s for SIGKILL
    Error = 'error'  # stopped and error occured (bpftrace error or process manager error)


class State:
    def __init__(self):
        self.status = Status.Stopped
        self.reset()

    def reset(self):
        self.pid = -1
        self.exit_code = 0
        self.error = ''
        self.probes = 0
        self.data = {}


class Script:
    def __init__(self, code: str):
        # PMNS metric names must start with an alphabetic character
        self.script_id = 's' + str(uuid.uuid4()).replace('-', '')
        self.username: str = None
        self.persistent = False
        self.created_at = datetime.now()
        self.last_accessed_at = datetime.now()
        self.code = code
        self.metadata = ScriptMetadata()
        self.variables: Dict[str, VariableDefinition] = {}
        self.state = State()

    def ident(self) -> str:
        code_output = self.code
        if len(code_output) > 80:
            code_output = code_output[:80-6] + ' [...]'
        code_output = code_output.replace('\n', '\\n')
        return f"BPFtrace (code='{code_output}', PID={self.state.pid})"


class ScriptEncoder(json.JSONEncoder):
    # pylint:disable=arguments-differ,method-hidden
    def default(self, obj):
        if isinstance(obj, datetime):
            return obj.isoformat()
        else:
            return obj.__dict__
        return json.JSONEncoder.default(self, obj)


class PMDAConfig:
    def __init__(self):
        # see bpftrace.conf for configuration descriptions and units
        self.bpftrace_path = 'bpftrace'
        self.script_expiry_time = 10 * 60  # 10 min
        self.max_throughput = 100 * 1024  # 100 KiB/s
        self.allowed_users = ['admin']


class RuntimeInfo:
    def __init__(self):
        # assuming latest version per default (simplifies version checks)
        self.bpftrace_version = (999, 999, 999)
        self.bpftrace_version_str = ""


class Logger:
    def __init__(self, info, error):
        self.info = info
        self.error = error
