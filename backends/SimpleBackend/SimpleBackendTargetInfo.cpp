#include "SimpleTargetMachine.h"
#include "llvm/Module.h"
#include "llvm/Support/TargetRegistry.h"

namespace llvm {
	Target TheSimpleBackendTarget;

	extern "C" void LLVMInitializeSimpleBackendTargetInfo() {
		RegisterTarget <> X(TheSimpleBackendTarget, "simple",
				    "SIMPLE backend");
}}
