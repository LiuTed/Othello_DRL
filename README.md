# Othello_DRL
Othello AI using deep reinforcement learning with method of Alpha Zero

https://botzone.org.cn/game/Reversi

## Model
- Given the resource limit on botzone, and the simplicity of this game, Othello_DRL use a simple ResNet-11 as its feature extractor. The output will be passed to two dense layer which outputs the probability (policy network) and value (value network) respectively until a time limit or loop limit reached.

- When playing, Monte-Carlo Tree Search will be performed. It will execute 'Selection-Expansion-Backup' iteratively.
    - At each time expand sub-trees, the network will be evaluated, and the policy network's output will be used as a priori probability of this action.
    - When backup, the value output of network will be added to all its ancestor nodes.
    - MCTS will select the most visited node as its next move.

- Network structure is constructed using python, and it is trained using tensorflow C++

## Build
- MCTS-only version:
    - Modify the `CC` or `CXX` in the file `Makefile`
    - `make` to build a playable executable file

- DRL version:
    - Install `bazel`
    - Clone `tensorflow` repo and config
    - Put this folder under `tensorflow/tensorflow/cc`
    - Run `bazel build [target]` to build:
        - `othello_train.exe` to train the network
        - `othello_test.exe` to test the network's performance

- bot.cpp that can be uploaded to Botzone:
    - Run `make bot.cpp`
