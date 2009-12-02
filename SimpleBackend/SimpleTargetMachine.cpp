#include "SimpleTargetMachine.h"

namespace llvm {

	bool SimpleTargetMachine::
	    addPassesToEmitWholeFile(PassManager & PM,
				     formatted_raw_ostream & o,
				     CodeGenFileType FileType,
				     CodeGenOpt::Level OptLevel) {
		PM.add(createGCLoweringPass());
		PM.add(createLowerAllocationsPass(true));
		PM.add(createLowerInvokePass());
		PM.add(createCFGSimplificationPass());	// clean up after lower invoke.
		PM.add(new
		       SimpleBackendNameAllUsedStructsAndMergeFunctions());
		PM.add(new SimpleWriter(o));
		PM.add(createGCInfoDeleter());
		return false;
}}
