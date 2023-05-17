load("@rules_cc//cc:defs.bzl", "cc_library")
cc_library(
    name = "caccore",
    srcs = ["cacCore.cxx"],
    hdrs = [
        "cacCore.h", 
        "register.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@cvm//:plusargs",
        "@fmt//:fmt",
        ":registerBuild",
        ":caclibBuild"
    ],
)

cc_library(
    name = "registerBuild",
    srcs = ["register.cxx"],
    hdrs = [
        "register.h", 
        "caclib.h",
    ],
    deps = [
        ":caclibBuild",
        "@fmt//:fmt",
    ]
)

cc_library(
    name = "caclibBuild",
    hdrs = [
        "caclib.h",
    ],
    deps = [
        "//src/lib:cacLibInclude",
    ],    
)
