#include "42TargetMachine.h"
#include "llvm/Module.h"
#include "llvm/Support/TargetRegistry.h"

namespace llvm {
	Target The42BackendTarget;

	extern "C" void LLVMInitialize42BackendTargetInfo() {
		RegisterTarget <> X(The42BackendTarget, "42",
				    "42 backend");
}}
