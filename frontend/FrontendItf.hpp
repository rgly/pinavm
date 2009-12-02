#ifndef _FRONTEND_H
#define _FRONTEND_H

#include <string>

#include "FrontendPass.hpp"
#include "Frontend.hpp"

#include "llvm/Module.h"
#include "llvm/Support/PassNameParser.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetSelect.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/PassManager.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/StandardPasses.h"
#include "llvm/Support/SystemUtils.h"
#include "llvm/Support/PassNameParser.h"
#include "llvm/Bitcode/ReaderWriter.h"

#include "config.h"

Frontend *launch_frontend(std::string InputFilename)
{
	Module *M;

	LLVMContext & Context = getGlobalContext();

	std::string ErrorMessage;

	if (MemoryBuffer * Buffer
	    = MemoryBuffer::getFileOrSTDIN(InputFilename, &ErrorMessage)) {
		M = ParseBitcodeFile(Buffer, Context, &ErrorMessage);
		delete Buffer;
	} else {
		ERROR("Not able to initialize module from bitcode\n");
	}

	// Create a PassManager to hold and optimize the collection of passes we are
	// about to build...
	//
	PassManager Passes;

	// Add an appropriate TargetData instance for this module...
	TargetData *td = new TargetData(M);
	Passes.add(td);

	// Check that the module is well formed on completion of optimization
	FunctionPass *vp = createVerifierPass();
	Passes.add(vp);

	// Pinapa pass
	FrontendPass *fep = new FrontendPass();
	Passes.add(fep);

	// Write bitcode out to disk or outs() as the last step...
	//Passes.add(createBitcodeWriterPass(*Out));

	// Now that we have all of the passes ready, run them.
	Passes.run(*M);

	return fep->getFrontend();
}

#endif
