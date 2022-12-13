load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

cvm_hash="dcd72e384bf9b66b708ad54dc48afcd4bf20abb0"
maybe(
    http_archive,
    name = "cvm",
    sha256 = "d4a4210c6d685d64c827d45fededfe07efec3af5ebd0132a35e6793eeee4922c",
    strip_prefix = "cvm-{commit}".format(commit=cvm_hash),
    url = "https://aus-gitlab.local.tenstorrent.com/riscv/dv/cvm/-/archive/{commit}/cvm-{commit}.tar.bz2".format(commit=cvm_hash),
)

git_repository(
    name = "googletest",
    remote = "https://github.com/google/googletest",
    #commit = "1b18723e874b256c1e39378c6774a90701d70f7a"
    tag = "release-1.11.0",
)
