#ifndef LLBO_INSTRUMENTATION_INSTRUMENTATION_H
#define LLBO_INSTRUMENTATION_INSTRUMENTATION_H

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include <algorithm>
#include <any>
#include <concepts>
#include <cstddef>
#include <iostream>

void helloWorld();

/**
 * @brief a type that's passed to instrumentor
 *
 * @tparam T must be llvm::Module or llvm::Function, the type should be
 allowed
 * to write a LLVM Pass.
 */
template <typename T>
concept Instrumentable = std::is_same<T, llvm::Module>::value ||
    std::is_same<T, llvm::Function>::value;

template <Instrumentable T> class Instrumentor {
public:
  explicit Instrumentor<T>() = default;
  void instrument(T& target);
};

template <> class Instrumentor<llvm::Module> : public llvm::ModulePass {
private:
  llvm::legacy::PassManager pm;

public:
  bool runOnModule(llvm::Module& m) override;
  void instrument(llvm::Module& target) {
    pm.add(this);
    pm.run(target);
  }
};

#endif