#include "llbo-instrument/instrumentation.h"
#include "llvm/IR/LLVMContext.h"

void helloWorld() { std::cout << "Hello World!" << std::endl; }

bool Instrumentor<llvm::Module>::runOnModule(llvm::Module& m) {
  llvm::LLVMContext& context = m.getContext();
  return true;
}
