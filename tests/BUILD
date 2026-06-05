load("@rules_cc//cc:defs.bzl", "cc_test")

cc_library(
    name = "arg_mock_lib",
    srcs = ["arg_mock.cxx"],
    deps = [
        "@cvm//third_party/vpi:vpi",
        "@cvm//:plusargs",
    ],
)

cc_test(
    name = 'cac_test',
    srcs = ["cac_test.cxx"],
    deps = [
        ":arg_mock_lib",
        "//src:cac_core",
        "@googletest//:gtest_main",
    ]
)
