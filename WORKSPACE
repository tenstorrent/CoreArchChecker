load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

cvm_hash="f9b0323a53714f69029a2e45958f3e322a33b527"
maybe(
    http_archive,
    name = "cvm",
    sha256 = "28bb5d51617e744c67e43cbf02b67a07afb998695554083a0b0c45605e5f0d9f",
    strip_prefix = "cvm-{commit}".format(commit=cvm_hash),
    url = "https://aus-gitlab.local.tenstorrent.com/riscv/dv/cvm/-/archive/{commit}/cvm-{commit}.tar.bz2".format(commit=cvm_hash),
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