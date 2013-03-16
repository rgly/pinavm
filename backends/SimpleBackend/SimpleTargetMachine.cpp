#include "SimpleTargetMachine.h"
#include "llvm/PassManager.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/Transforms/Scalar.h"
#include "SimpleWriter.h"

namespace llvm {

	bool SimpleTargetMachine::
	    addPassesToEmitWholeFile(PassManager & PM,
				     formatted_raw_ostream & o,
				     CodeGenFileType FileType,
				     CodeGenOpt::Level OptLevel,
				     bool DisableVerify) {
		PM.add(createGCLoweringPass());
		PM.add(createLowerInvokePass());
		PM.add(createCFGSimplificationPass());	// clean up after lower invoke.
		//PM.add(new SimpleBackendNameAllUsedStructsAndMergeFunctions());
		PM.add(new SimpleWriter(o));
		PM.add(createGCInfoDeleter());
		return false;
}}
