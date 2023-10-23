#include "llbo-instrument/instrumentation.h"
#include "llbo-utils/code_gen.h"

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/TargetSelect.h"
#include <memory>
#include <string>
#include <vector>

static llvm::cl::opt<std::string> filePath{
    llvm::cl::Positional,
    llvm::cl::desc{"<Module to analyze>"},
    llvm::cl::init(""),
    llvm::cl::Required,
};

static llvm::cl::opt<std::string> outFile{
    "o",
    llvm::cl::desc{"<Output file that would have been instrumented>"},
    llvm::cl::value_desc{"filename"},
    llvm::cl::init(""),
};

static llvm::cl::list<std::string> libPaths{
    "L",
    llvm::cl::Prefix,
    llvm::cl::desc{"<Library search path>"},
    llvm::cl::value_desc{"directory"},
};

static llvm::cl::list<std::string> libraries{
    "l",
    llvm::cl::Prefix,
    llvm::cl::desc{"<Libraries to link>"},
    llvm::cl::value_desc{"library prefix"},
};

void instrument(llvm::Module* m) {
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::cl::AddExtraVersionPrinter(
      llvm::TargetRegistry::printRegisteredTargetsForVersion);

  if (outFile.getValue().empty()) {
    llvm::report_fatal_error("-o <output-file> must be specified.");
  }
  std::vector<std::string> lib_paths(libPaths.begin(), libPaths.end());
  std::vector<std::string> libs(libraries.begin(), libraries.end());
  llbo::LinkContext context(lib_paths, libs);
  std::cout << m << std::endl;
  llbo::CodeGenerator code_generator(m);
  code_generator.generateBinary(outFile.getValue(), context);
  llbo::ObjectWriter::writeModule(*m, outFile.getValue() + ".instrumented.bc");
}

int main(int argc, char** argv) {
  llvm::PrettyStackTraceProgram X(argc, argv);
  llvm::cl::ParseCommandLineOptions(argc, argv);

  llvm::SMDiagnostic err;
  llvm::LLVMContext contetxt;
  std::unique_ptr<llvm::Module> module =
      llvm::parseIRFile(filePath.getValue(), err, contetxt);
  llvm::Module* m = module.get();
  if (m == nullptr) {
    llvm::report_fatal_error(("Error parsing a bitcode file (" +
                              filePath.getValue() +
                              "): " + err.getMessage().str())
                                 .c_str());
  }
  instrument(m);
  return 0;
}
