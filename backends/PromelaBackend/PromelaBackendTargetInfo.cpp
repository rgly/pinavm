#include "PromelaTargetMachine.h"
#include "llvm/Module.h"
#include "llvm/Support/TargetRegistry.h"

namespace llvm {
	Target ThePromelaBackendTarget;

	extern "C" void LLVMInitializePromelaBackendTargetInfo() {
		RegisterTarget <> X(ThePromelaBackendTarget, "promela",
				    "PROMELA backend");
}}
