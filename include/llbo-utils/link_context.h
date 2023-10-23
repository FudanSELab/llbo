#include <sstream>
#include <string>
#include <vector>

#include "llvm/Support/CommandLine.h"

namespace llbo {
class LinkContext {
private:
  const std::vector<std::string> lib_paths;
  const std::vector<std::string> libraries;

public:
  explicit LinkContext(const std::vector<std::string> lib_paths,
                       const std::vector<std::string> libraries)
      : lib_paths(lib_paths), libraries(libraries) {}

  inline std::string dumpLinkArguments() const {
    std::ostringstream oss;
    for (auto& lib_path : this->lib_paths)
      oss << "-L" << lib_path << " ";
    for (auto& library : this->libraries)
      oss << "-l" << library << " ";
    return oss.str();
  }
};
} // namespace llbo
