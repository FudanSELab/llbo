#include "llbo-utils/code_gen.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/None.h"
#include "llvm/ADT/Triple.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/CodeGen/CommandFlags.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include <cassert>
#include <memory>
#include <system_error>
#include <vector>

namespace llbo {

// static fields of CodeGenerator
std::unique_ptr<llvm::TargetMachine> CodeGenerator::target_machine = nullptr;
llvm::codegen::RegisterCodeGenFlags CodeGenerator::cfg;

llvm::TargetMachine*
CodeGenerator::getTargetMachine(const llvm::Module* module) {
  // for caching
  if (CodeGenerator::target_machine)
    return CodeGenerator::target_machine.get();

  std::string err;
  const std::string target_triple = module->getTargetTriple();
  const std::string march = llvm::codegen::getMArch();
  const llvm::Triple triple = llvm::Triple(target_triple);
  if (target_triple.empty() || march.empty()) {
    llvm::report_fatal_error(
        ("target_triple(" + target_triple + ") or MArch(" + march + ") empty")
            .c_str());
  }

  const llvm::Target* target = llvm::TargetRegistry::lookupTarget(
      march, const_cast<llvm::Triple&>(triple), err);
  if (target == nullptr) {
    llvm::report_fatal_error(("Unable to find target: " + err).c_str());
  }

  // prepare arguments for TargetMachine constructor
  auto tmp_rm = llvm::codegen::getExplicitRelocModel();
  auto relocation_model = tmp_rm ? tmp_rm : llvm::Reloc::Model::PIC_;
  auto code_model = llvm::codegen::getExplicitCodeModel();
  llvm::TargetOptions options =
      llvm::codegen::InitTargetOptionsFromCodeGenFlags(triple);
  if (auto float_abi = llvm::codegen::getFloatABIForCalls();
      float_abi != llvm::FloatABI::Default) {
    options.FloatABIType = float_abi;
  }

  // create TargetMachine and cache
  std::unique_ptr<llvm::TargetMachine> tm(target->createTargetMachine(
      triple.getTriple(), llvm::codegen::getCPUStr(),
      llvm::codegen::getFeaturesStr(), options, relocation_model, code_model,
      llvm::CodeGenOpt::None));
  assert(tm != nullptr && "Could not allocate target machine");
  CodeGenerator::target_machine = std::move(tm);
  return CodeGenerator::target_machine.get();
}

bool CodeGenerator::compile(const std::string output_path) {
  llvm::cl::PrintOptionValues();
  // add target-library-info-wrapper pass
  std::string target_triple = this->module->getTargetTriple();
  const llvm::Triple triple = llvm::Triple(target_triple);
  const llvm::TargetLibraryInfoImpl tlii(triple);
  llvm::legacy::PassManager pm;
  pm.add(new llvm::TargetLibraryInfoWrapperPass(tlii));

  // set data layout
  llvm::TargetMachine* tm = getTargetMachine(this->module);
  const llvm::DataLayout& layout = tm->createDataLayout();
  this->module->setDataLayout(layout);

  // add emit-file pass
  std::error_code err_code;
  auto out = std::make_unique<llvm::ToolOutputFile>(output_path, err_code,
                                                    llvm::sys::fs::OF_None);
  llvm::raw_pwrite_stream* os(&out->os());
  if (tm->addPassesToEmitFile(pm, *os, nullptr, llvm::CGFT_ObjectFile)) {
    assert(false && "target does not support generation of this file type!");
  }

  return pm.run(*this->module);
}

bool CodeGenerator::link(const std::string output_path,
                         const std::string object_file_path,
                         const llbo::LinkContext context) {
  // prepare link command arguments
  auto clangpp_opt = llvm::sys::findProgramByName("clang++");
  assert(clangpp_opt && "cannot find clang++ by llvm::sys::findProgramByName");
  const std::string clangpp_path = clangpp_opt.get();
  const std::string link_arguments = context.dumpLinkArguments();

  std::vector<llvm::StringRef> args{
      clangpp_path, "-O0", "-o", output_path, object_file_path, link_arguments};
  for (auto& elem : args) {
    llvm::outs() << elem << " ";
  }
  llvm::outs() << "\n";

  // execute link phase
  if (-1 ==
      llvm::sys::ExecuteAndWait(clangpp_path, args, llvm::NoneType::None)) {
    llvm::report_fatal_error("Unable to link output file.");
  }
  return true;
}

bool CodeGenerator::generateBinary(const std::string output_path,
                                   const llbo::LinkContext context) {
  bool compile_ok = compile(output_path + ".o");
  assert(compile_ok && "Falied to compile.");
  bool link_ok = link(output_path, output_path + ".o", context);
  assert(link_ok && "Failed to link.");
  return true;
}

void ObjectWriter::writeModule(const llvm::Module& module,
                               llvm::StringRef file_path) {
  std::error_code errc;
  llvm::raw_fd_ostream out(file_path.data(), errc, llvm::sys::fs::OF_None);
  if (errc) {
    llvm::report_fatal_error(("error saving llvm module to '" +
                              file_path.str() + "': " + errc.message())
                                 .c_str());
  }

  llvm::WriteBitcodeToFile(module, out);
}
} // namespace llbo
