load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "cac_core",
    srcs = ["cac_core.cxx"],
    hdrs = [
        "cac_core.h", 
        "resource.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":cac_lib",
        ":resource",
        "@cvm//:bitmanip",
        "@cvm//:plusargs",
        "@fmt//:fmt",
    ],
)

cc_library(
    name = "resource",
    srcs = ["resource.cxx"],
    hdrs = [
        "resource.h", 
    ],
    deps = [
        ":cac_lib",
        "@fmt//:fmt",
    ]
)

cc_library(
    name = "cac_lib",
    hdrs = ["cac_lib.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@CoreArchChecker//src/lib:cac_lib_include"
    ]
)