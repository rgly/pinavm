#include "PromelaTargetMachine.h"
#include "llvm/Module.h"
#include "llvm/Support/TargetRegistry.h"
//#include "llvm/ADT/StringExtras.h"
//#include "llvm/Support/raw_ostream.h"
//#include <sstream>
//#include <string>
//#include <iostream>

namespace llvm {
	Target ThePromelaBackendTarget;

	extern "C" void LLVMInitializePromelaBackendTargetInfo() {
		RegisterTarget <> X(ThePromelaBackendTarget, "promela",
				    "PROMELA backend");
}}
