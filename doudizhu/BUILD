load("@rules_cc//cc:defs.bzl", "cc_test", "cc_library")

cc_library(
    name = "libdoudizhu.so",
    srcs = ["doudizhu_game.cc",
        "doudizhu_card.cc",
        "doudizhu_utils.cc",
        "doudizhu_action.cc"],
    hdrs = ["doudizhu_game.h",
        "doudizhu_types.h",
        "doudizhu_card.h",
        "doudizhu_utils.h",
        "doudizhu_action.h"],
    copts = ["-Iexternal/eigen"],
    deps = [
        "@eigen",
        "@spdlog"
    ],
    visibility = [
        "//mccfr:__pkg__"
    ]
)

cc_test(
  name = "doudizhu_test",
  srcs = ["doudizhu_test.cc"],
  data = [
      "type_card.json",
  ],
  deps = ["@gtest//:gtest_main",
        "@jsoncpp",
        ":libdoudizhu.so"],
)
