# Compatibility shim providing the `@rules_hdl//verilog:providers.bzl` surface
# that cvm's BUILD files still load (`verilog_library`). CoreArchChecker only
# consumes cvm's C++ libraries (plusargs, bitmanip, vpi) and never analyzes a
# verilog_library target, so this rule only needs to be loadable/definable —
# its implementation is never evaluated here.
#
# This is a trimmed variant of the shim in the mem-manager repo: the upstream
# @rules_verilog VerilogInfo emission (needed only when @rules_verilator
# consumes verilog_library aggregators) is omitted, so this repo needs no
# @rules_verilog / bazel_skylib wiring. If CoreArchChecker ever adds Verilator
# targets, port mem-manager's full shim (and its @rules_verilog wiring) instead.
#
# Delete this directory (along with the @rules_hdl wiring in WORKSPACE) once
# downstream consumers migrate off the legacy @rules_hdl surface.

VerilogInfo = provider(
    doc = "Legacy DAG-based VerilogInfo. Provider identity must stay stable for downstream compatibility.",
    fields = {
        "dag": "depset of DAG entries (struct of srcs/hdrs/libs/fs/views/deps/label/strip_include_prefix).",
        "plis": "depset (unused today; kept for downstream field compatibility).",
    },
)

def make_dag_entry(srcs, hdrs, libs, fs, views, deps, label, strip_include_prefix):
    return struct(
        srcs = tuple(srcs),
        hdrs = tuple(hdrs),
        libs = tuple(libs),
        fs = tuple(fs),
        views = tuple(views),
        deps = tuple(deps),
        label = label,
        strip_include_prefix = strip_include_prefix,
    )

def make_verilog_info(new_entries = (), old_infos = ()):
    return VerilogInfo(
        dag = depset(
            direct = new_entries,
            order = "postorder",
            transitive = [x.dag for x in old_infos],
        ),
        plis = depset(),
    )

def _verilog_library_impl(ctx):
    info = make_verilog_info(
        new_entries = [make_dag_entry(
            srcs = ctx.files.srcs,
            hdrs = ctx.files.hdrs,
            libs = ctx.files.libs,
            fs = ctx.files.fs,
            views = ctx.attr.views,
            deps = ctx.attr.deps,
            label = ctx.label,
            strip_include_prefix = ctx.attr.strip_include_prefix,
        )],
        old_infos = [dep[VerilogInfo] for dep in ctx.attr.deps],
    )
    return [info, DefaultInfo(files = depset(ctx.files.srcs + ctx.files.hdrs))]

verilog_library = rule(
    attrs = {
        "srcs": attr.label_list(allow_files = True),
        "hdrs": attr.label_list(allow_files = [".svh", ".vh"]),
        "libs": attr.label_list(allow_files = True),
        "fs": attr.label_list(allow_files = True),
        "views": attr.string_list(),
        "deps": attr.label_list(providers = [VerilogInfo]),
        "strip_include_prefix": attr.string(),
        "top_module": attr.string(default = ""),
    },
    implementation = _verilog_library_impl,
)
