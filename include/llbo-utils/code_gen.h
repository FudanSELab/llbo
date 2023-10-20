#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/CommandFlags.h"
#include "llvm/IR/Module.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Target/TargetMachine.h"
#include <memory>

namespace llbo {
class CodeGenerator {
private:
  llvm::Module* module;

  static std::unique_ptr<llvm::TargetMachine> target_machine;
  static llvm::codegen::RegisterCodeGenFlags cfg;

  static llvm::TargetMachine* getTargetMachine(const llvm::Module* module);

public:
  explicit CodeGenerator(llvm::Module* module) : module(module) {}

  bool compile(const std::string output_path);
  bool link(const std::string output_path, const std::string object_file_path);
  bool generate_binary(const std::string output_path);
};

class ObjectWriter {
public:
  static void write_module(const llvm::Module& module,
                           llvm::StringRef file_path);
};
} // namespace llbo
