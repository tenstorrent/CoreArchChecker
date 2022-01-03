load("@rules_cc//cc:defs.bzl", "cc_library")
cc_library(
    name = "cboardcore",
    srcs = ["cboard.cxx"],
    hdrs = [
        "cboard.h", 
        "register.h",
        "info.h",
        "cboardTypeDef.h", 
        "stateIdDef.h", 
        "external.h", 
        "cboardConst.h"
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":registerBuild",
        ":infoBuild",
    ],
)

cc_library(
    name = "registerBuild",
    srcs = ["register.cxx"],
    hdrs = [
        "register.h", 
        "cboardTypeDef.h", 
        "stateIdDef.h", 
        "external.h", 
        "cboardConst.h"
    ]
)

cc_library(
    name = "infoBuild",
    srcs = ["info.cxx"],
    hdrs = [
        "info.h", 
        "cboardTypeDef.h", 
        "stateIdDef.h", 
        "external.h", 
        "cboardConst.h"
    ]
)