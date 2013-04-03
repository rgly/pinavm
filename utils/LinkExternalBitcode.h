#include <string>
namespace llvm { struct Module;};
extern llvm::Module* LinkExternalBitcode(llvm::Module*, std::string);
