#ifndef _FRONTEND_H
#define _FRONTEND_H

#include <string>

#include "FrontendPass.hpp"
#include "Frontend.hpp"

//#include "llvm/Transforms/Scalar.h"
#include "llvm/Module.h"
//#include "llvm/Support/PassNameParser.h"
//#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/DataLayout.h"
//#include "llvm/Target/TargetMachine.h"
//#include "llvm/Target/TargetSelect.h"
//#include "llvm/Support/ManagedStatic.h"
#include "llvm/PassManager.h"
//#include "llvm/Support/MemoryBuffer.h"
//#include "llvm/Support/SystemUtils.h"
//#include "llvm/Support/PassNameParser.h"
//#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Transforms/Scalar.h"

#include "config.h"

Frontend *launch_frontend(std::string InputFilename, bool inlineFcts,Module *Mod)
{
	//Module *Mod;
	LLVMContext & Context = getGlobalContext();

	std::string ErrorMessage;

	/*if (MemoryBuffer * Buffer
	    = MemoryBuffer::getFileOrSTDIN(InputFilename, &ErrorMessage)) {
		Mod = ParseBitcodeFile(Buffer, Context, &ErrorMessage);
		delete Buffer;
	} else {
		ERROR("Not able to initialize module from bitcode\n");
	}*/

	// Create a PassManager to hold and optimize the collection of passes we are
	// about to build...
	//
	PassManager Passes;

	// Add an appropriate DataLayout instance for this module...
	DataLayout *dl = new DataLayout(Mod);
// 	Passes.add(createLoopSimplifyPass());
// 	Passes.add(createLoopUnrollPass());
	Passes.add(dl);

	// Check that the module is well formed on completion of optimization
	FunctionPass *vp = createVerifierPass();
	Passes.add(vp);

//	Passes.add(createGCLoweringPass());
	//	Passes.add(createLowerAllocationsPass(true));
	Passes.add(createLowerInvokePass());
	Passes.add(createCFGSimplificationPass());	// clean up after lower invoke.

	TRACE_3("foo\n");
	// PinaVM pass
	FrontendPass *fep = new FrontendPass();
	fep->setInlineFunctions(inlineFcts);
	Passes.add(fep);
	TRACE_3("bar\n");
	
	// Write bitcode out to disk or outs() as the last step...
	//Passes.add(createBitcodeWriterPass(*Out));

	// Now that we have all of the passes ready, run them.
	Passes.run(*Mod);
	TRACE_3("gee\n");

	return fep->getFrontend();
}

#endif
