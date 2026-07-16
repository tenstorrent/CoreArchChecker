workspace(name = "CoreArchChecker")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")


cvm_hash="8f0aaae6ea50ac26061caa8160fa18721c216f62"
#   curl -sSL https://github.com/tenstorrent/cvm/archive/<commit>.tar.gz | sha256sum
maybe(
    http_archive,
    name = "cvm",
    sha256 = "0000000000000000000000000000000000000000000000000000000000000000",
    strip_prefix = "cvm-{commit}".format(commit=cvm_hash),
    url = "https://github.com/tenstorrent/cvm/archive/{commit}.tar.gz".format(commit=cvm_hash),
)

git_repository(
    name = "googletest",
    remote = "https://github.com/google/googletest",
    #commit = "1b18723e874b256c1e39378c6774a90701d70f7a"
    tag = "release-1.11.0",
)

load("@cvm//deps:repositories.bzl", "cvm_dependencies")
cvm_dependencies()

load("@cvm//deps:toolchains1.bzl", "cvm_toolchains1")
cvm_toolchains1()

load("@cvm//deps:toolchains2.bzl", "cvm_toolchains2")
cvm_toolchains2()
