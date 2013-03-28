#include "42TargetMachine.h"
#include "llvm/Module.h"
#include "llvm/Support/TargetRegistry.h"
//#include "llvm/ADT/StringExtras.h"
//#include "llvm/Support/raw_ostream.h"
//#include <sstream>
//#include <string>
//#include <iostream>

namespace llvm {
	Target The42BackendTarget;

	extern "C" void LLVMInitialize42BackendTargetInfo() {
		RegisterTarget <> X(The42BackendTarget, "42",
				    "42 backend");
}}
