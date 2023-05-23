load("@rules_cc//cc:defs.bzl", "cc_test")

cc_library(
    name = "argMockLib",
    srcs = ["argMock.cxx"],
    deps = [
        "@cvm//third_party/vpi:vpi",
        "@cvm//:plusargs",
    ]
)

cc_test(
    name = 'cacTest',
    srcs = ["cacTest.cxx"],
    deps = [
        "//src:caccore",
        ":argMockLib",
        "@cvm//:plusargs",
        "@googletest//:gtest_main",
    ]
)
