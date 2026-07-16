workspace(name = "CoreArchChecker")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")


cvm_hash="f32e669c55866f2f01c39be00b208535c2737d1c"
maybe(
    http_archive,
    name = "cvm",
    sha256 = "43880b59b2d6c54391285afe0172b3e10769f2512f19b174dd17275e7c1d6d87",
    strip_prefix = "CVM-{commit}".format(commit=cvm_hash),
    url = "https://github.com/tenstorrent/cvm/archive/{commit}.tar.gz".format(commit=cvm_hash),
)

git_repository(
    name = "googletest",
    remote = "https://github.com/google/googletest",
    #commit = "1b18723e874b256c1e39378c6774a90701d70f7a"
    tag = "release-1.11.0",
)

# Local compat shim publishing the @rules_hdl//verilog:providers.bzl surface
# that cvm's BUILD files load. CoreArchChecker only uses cvm's C++ libraries
# and never analyzes a verilog_library target, so the shim just needs to be
# loadable (see bazel/rules_hdl_compat/verilog/providers.bzl).
local_repository(
    name = "rules_hdl",
    path = "bazel/rules_hdl_compat",
)

load("@cvm//deps:repositories.bzl", "cvm_dependencies")
cvm_dependencies()

load("@cvm//deps:toolchains1.bzl", "cvm_toolchains1")
cvm_toolchains1()

load("@cvm//deps:toolchains2.bzl", "cvm_toolchains2")
cvm_toolchains2()
