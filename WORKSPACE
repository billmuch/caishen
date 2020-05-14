workspace(name = "caishen")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Eigen
http_archive(
    name = "eigen",
    build_file = "//third_party:eigen.BUILD",
    sha256 = "88e95180a7eae9acd3e79d2efeea1026eefad9f515a44418b63b189a1887108c",  # SHARED_EIGEN_SHA
    strip_prefix = "eigen-52a2fbbb008a47c5e3fb8ac1c65c2feecb0c511c",
    url = "https://gitlab.com/libeigen/eigen/-/archive/52a2fbbb008a47c5e3fb8ac1c65c2feecb0c511c/eigen-52a2fbbb008a47c5e3fb8ac1c65c2feecb0c511c.tar.gz",
)

# jsoncpp
http_archive(
    name = "jsoncpp",
    build_file = "//third_party:jsoncpp.BUILD",
    sha256 = "77a402fb577b2e0e5d0bdc1cf9c65278915cdb25171e3452c68b6da8a561f8f0",
    strip_prefix = "jsoncpp-1.9.2",
    url = "https://github.com/open-source-parsers/jsoncpp/archive/1.9.2.tar.gz", 
)

http_archive(
    name = "gtest",
    sha256 = "ff7a82736e158c077e76188232eac77913a15dac0b22508c390ab3f88e6d6d86",
    strip_prefix = "googletest-b6cd405286ed8635ece71c72f118e659f4ade3fb",
    urls = [
        "https://github.com/google/googletest/archive/b6cd405286ed8635ece71c72f118e659f4ade3fb.zip",
    ],
)
