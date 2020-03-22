load(
    "//tensorflow:tensorflow.bzl",
    "tf_cc_binary",
    "tf_copts"
)

tf_cc_binary(
    name = "othello.exe",
    srcs = [
        "include/jsoncpp/json.h",
        "Net.cpp",
        "Othello.cpp",
        "MCTS.cpp",
        "MCTS_DRL.cpp",
        "main.cpp"
    ],
    copts = tf_copts() + ["-Itensorflow/cc/othello_drl/include"],
    deps = [
        "//tensorflow/cc:cc_ops",
        "//tensorflow/core:core_cpu",
        "//tensorflow/core:framework",
        "//tensorflow/core:lib",
        "//tensorflow/core:protos_all_cc",
        "//tensorflow/core:tensorflow",
    ],
)

tf_cc_binary(
    name = "othello_train.exe",
    srcs = [
        "include/jsoncpp/json.h",
        "include/Othello.h",
        "include/MCTS.h",
        "include/MCTS_DRL.h",
        "include/Net.h",
        "Net.cpp",
        "Othello.cpp",
        "MCTS.cpp",
        "MCTS_DRL.cpp",
        "train.cpp"
    ],
    copts = tf_copts() + ["-Itensorflow/cc/othello_drl/include"],
    deps = [
        "//tensorflow/cc:cc_ops",
        "//tensorflow/core:core_cpu",
        "//tensorflow/core:framework",
        "//tensorflow/core:lib",
        "//tensorflow/core:protos_all_cc",
        "//tensorflow/core:tensorflow",
    ],
)

tf_cc_binary(
    name = "othello_test.exe",
    srcs = [
        "include/jsoncpp/json.h",
        "include/Othello.h",
        "include/MCTS.h",
        "include/MCTS_DRL.h",
        "include/Net.h",
        "Net.cpp",
        "Othello.cpp",
        "MCTS.cpp",
        "MCTS_DRL.cpp",
        "test.cpp"
    ],
    copts = tf_copts() + ["-Itensorflow/cc/othello_drl/include"],
    deps = [
        "//tensorflow/cc:cc_ops",
        "//tensorflow/core:core_cpu",
        "//tensorflow/core:framework",
        "//tensorflow/core:lib",
        "//tensorflow/core:protos_all_cc",
        "//tensorflow/core:tensorflow",
    ],
)

