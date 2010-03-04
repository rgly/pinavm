#include "PromelaTargetMachine.h"
#include "llvm/PassManager.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/Transforms/Scalar.h"
#include "PromelaWriter.h"

namespace llvm {

	bool PromelaTargetMachine::
	    addPassesToEmitWholeFile(PassManager & PM,
				     formatted_raw_ostream & o,
				     CodeGenFileType FileType,
				     CodeGenOpt::Level OptLevel) {
		PM.add(createGCLoweringPass());
		PM.add(createLowerAllocationsPass(true));
		PM.add(createLowerInvokePass());
		PM.add(createCFGSimplificationPass());	// clean up after lower invoke.
		PM.add(new
		       PromelaBackendNameAllUsedStructsAndMergeFunctions());
		PM.add(new PromelaWriter(o));
		PM.add(createGCInfoDeleter());
		return false;
}}
