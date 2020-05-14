# Caishen

A Research on Doudizhu Poker Game

## Prerequisites

- [bazel](https://www.bazel.build/versions/master/docs/install.html)

## Directory Structure

```
caishen
 └── doudizhu                   # Doudizhu Game Engine.
```

## Build Doudizhu 

1. Git clone the source code
2. Build the Doudizhu game C library

    ```sh
    bazel build -c opt //doudizhu:libdoudizhu.so
    ```

3. [optional] Run unittest

    ```sh
    bazel test //doudizhu:doudizhu_test
    ```
