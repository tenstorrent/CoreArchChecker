load("@testgen//:defs.bzl", rr_testlist = "testlist")

TESTLISTS = {
    "smoke": {
        "testlist" : "//dv/testlists:smoke.py",
    },
    "rpc_smoke": {
        "testlist" : "@risc-p-cores//dv/core/testlists:smoke.py",
        "args"     : ["--testlist-arg=--tb=dv:risc_p_cores", "--test-executor-arg=--dbg=dbg"],
        "workspace": "@risc-p-cores",
    },
}

def _testlist(name, testlist, workspace, **kwargs):

    rr_testlist(
        name = name,
        testlist = testlist,
        workspace = workspace,
        **kwargs,
    )

def load_testlists():

    for name,values in TESTLISTS.items():
        _testlist(
            name = name,
            testlist = values['testlist'],
            args = values.get('args', []),
            workspace = values.get('workspace', '@chips'),
        )
