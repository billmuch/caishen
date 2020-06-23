load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_library(
    name = "libmccfr.so",
    srcs = ["mccfr.cc",
            "mem_informationset_store.cc",
            "bucket_actions_nohistory_nopublic.cc"],
    hdrs = ["mccfr.h",
            "mem_informationset_store.h",
            "cfr_doudizhu_player.h",
            "bucket_actions_nohistory_nopublic.h"],
    copts = ["-Iexternal/eigen"],
    deps= [
        "@eigen",
        "@spdlog",
        "//doudizhu:libdoudizhu.so"
    ],
)

cc_test(
  name = "mccfr_test",
  srcs = ["mccfr_test.cc",
          "banhnp_player_test.cc"],
  deps = ["@gtest//:gtest_main",
        ":libmccfr.so"],
  linkopts = ["-lstdc++fs",]
)

cc_binary(
  name = "mccfr_train",
  srcs = ["mccfr_train.cc"],
  deps = [":libmccfr.so",
        "//doudizhu:libdoudizhu.so"],
  linkopts = ["-lstdc++fs",]
)