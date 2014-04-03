#include <string>
namespace llvm {
	class Module;
}
extern llvm::Module* LinkExternalBitcode(llvm::Module*, std::string);

