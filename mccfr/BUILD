load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_library(
    name = "libmccfr.so",
    srcs = ["mccfr.cc",
            "bucket_actions_nohistory_nopublic.cc"],
    hdrs = ["mccfr.h",
            "cfr_doudizhu_player.h",
            "bucket_actions_nohistory_nopublic.h"],
    copts = ["-Iexternal/eigen"],
    deps= [
        "@eigen",
        "//doudizhu:libdoudizhu.so"
    ],
)

cc_test(
  name = "mccfr_test",
  srcs = ["mccfr_test.cc"],
  deps = ["@gtest//:gtest_main",
        ":libmccfr.so"],
)

cc_binary(
  name = "mccfr",
  srcs = ["main.cc"],
  deps = [":libmccfr.so",
        "//doudizhu:libdoudizhu.so"],
)