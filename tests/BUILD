load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = 'basicClassTest',
    srcs = ["basicClassTest.cxx"],
    deps = [
        "//src:cboardcore",
        "@googletest//:gtest_main",
    ]
)

cc_test(
    name = 'testCBoard',
    srcs = ["testCBoard.cxx"],
    deps = [
        "//src:cboardcore",
        "@googletest//:gtest_main",
    ]
)