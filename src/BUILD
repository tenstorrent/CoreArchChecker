load("@rules_cc//cc:defs.bzl", "cc_library")
cc_library(
    name = "cboardcore",
    srcs = ["cboard.cxx"],
    hdrs = [
        "cboard.h", 
        "register.h",
        "info.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":registerBuild",
        ":infoBuild",
        ":cboardlibBuild"
    ],
)

cc_library(
    name = "registerBuild",
    srcs = ["register.cxx"],
    hdrs = [
        "register.h", 
        "cboardlib.h",
    ],
    deps = [
        ":cboardlibBuild"
    ]
)

cc_library(
    name = "infoBuild",
    srcs = ["info.cxx"],
    hdrs = [
        "info.h", 
        "cboardlib.h",
    ],
    deps = [
        ":cboardlibBuild"
    ]
)

cc_library(
    name = "cboardlibBuild",
    hdrs = [
        "cboardlib.h",
    ],
    deps = [
        "//src/lib:cbdLibInclude",
    ],    
)
