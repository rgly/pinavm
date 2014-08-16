#include <string>
namespace llvm {
	class Module;
}
extern llvm::Module* loadExternalBitcode(std::string);
extern void LinkExternalBitcode(llvm::Module*, std::string);

