#include "42TargetMachine.h"
#include "llvm/PassManager.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/Transforms/Scalar.h"
#include "42Writer.h"

namespace llvm {

	bool _42TargetMachine::
	    addPassesToEmitWholeFile(PassManager & PM,
				     formatted_raw_ostream & o,
				     CodeGenFileType FileType,
				     CodeGenOpt::Level OptLevel,
				     bool DisableVerify) {
		PM.add(createGCLoweringPass());
		PM.add(createLowerInvokePass());
		PM.add(createCFGSimplificationPass());	// clean up after lower invoke.
		//PM.add(new _42BackendNameAllUsedStructsAndMergeFunctions());
		PM.add(new _42Writer(o));
		PM.add(createGCInfoDeleter());
		return false;
}}
