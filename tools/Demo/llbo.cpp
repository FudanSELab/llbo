#include "llbo-instrument/instrumentation.h"

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/SourceMgr.h"
#include <memory>
#include <string>

static llvm::cl::opt<std::string> filePath{
    llvm::cl::Positional,
    llvm::cl::desc{"<Module to analyze>"},
    llvm::cl::init(""),
    llvm::cl::Required,
};

int main() {
  helloWorld();

  llvm::SMDiagnostic err;
  llvm::LLVMContext contetxt;
  std::unique_ptr<llvm::Module> module =
      llvm::parseIRFile(filePath.getValue(), err, contetxt);
  if (!module.get()) {
    llvm::errs() << "Error parsing a bitcode file (" << filePath.getValue()
                 << ")\n";
    return -1;
  }

  return 0;
}
