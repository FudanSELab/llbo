# llbo (Buffer Overflow Detection based on LLVM)

## 1. requirement

* LLVM 14.0
* C++ 20

## 2. build

```sh
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -DCMAKE_C_COMPILER=/usr/bin/clang -DLLVM_DIR="path to LLVMConfig.cmake"
cmake --build build
```

e.g.

```sh
cmake -S . -B build -GNinja -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -DCMAKE_C_COMPILER=/usr/bin/clang  -DLLVM_DIR="/home/lijg/llvm-15-0-6/llvm-install/lib/cmake/llvm"
cmake --build build
```
