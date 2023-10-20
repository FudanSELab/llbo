# llbo (Buffer Overflow Detection based on LLVM)

## 1. build

```sh
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -DCMAKE_C_COMPILER=/usr/bin/clang -DLLVM_DIR="path to LLVMConfig.cmake"
cmake --build build
```
