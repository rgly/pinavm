#include "SimpleTargetMachine.h"
#include "llvm/Module.h"
#include "llvm/Support/TargetRegistry.h"
//#include "llvm/ADT/StringExtras.h"
//#include "llvm/Support/raw_ostream.h"
//#include <sstream>
//#include <string>
//#include <iostream>

namespace llvm {
	Target TheSimpleBackendTarget;

	extern "C" void LLVMInitializeSimpleBackendTargetInfo() {
		RegisterTarget <> X(TheSimpleBackendTarget, "simple",
				    "SIMPLE backend");
}}
