#include "PromelaTargetMachine.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"

namespace llvm {
	Target ThePromelaBackendTarget;

	extern "C" void LLVMInitializePromelaBackendTargetInfo() {
		RegisterTarget <> X(ThePromelaBackendTarget, "promela",
				    "PROMELA backend");
}}
