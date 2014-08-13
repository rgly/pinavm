#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/Signals.h"
#include "llvm/IR/DataLayout.h"

#include "Frontend.hpp"
#include "42Writer.h"
#include "42Backend.h"

#include "OldVersion.h"
using namespace llvm;

void launch_42backend(Frontend * fe,
			std::string OutputFilename,
			bool useBoolInsteadOfInts,
			bool relativeClocks,
			bool bug)
{
	Module *llvmMod = fe->getLLVMModule();

	// Figure out what stream we are supposed to write to...
	// FIXME: outs() is not binary!
	formatted_raw_ostream *Out = &fouts();

	if (OutputFilename != "-") {

		std::string error;
		raw_fd_ostream *FDOut = new raw_fd_ostream(OutputFilename.c_str(), error, sys::fs::F_None);
		if (!error.empty()) {
			errs() << error << '\n';
			delete FDOut;
			return;
		}
		Out = new formatted_raw_ostream(*FDOut,	formatted_raw_ostream::DELETE_STREAM);

		// Make sure that the Output file gets unlinked from the disk if we get a
		// SIGINT
		sys::RemoveFileOnSignal(OutputFilename);
	}
	// Build up all of the passes that we want to do to the module.
	PassManager Passes;

	ModulePass * FortyTwoWriter = new _42Writer(fe, *Out, useBoolInsteadOfInts, relativeClocks, bug);

	// To compatiable with legacy passmanager. DataLayout is not
	// a pass anymore.
	DataLayout* dl = new DataLayout(llvmMod);
	DataLayoutPass* dlpass = new DataLayoutPass(*dl);

	Passes.add(dlpass);
	Passes.add(createVerifierPass());
	Passes.add(createGCLoweringPass());
	Passes.add(createLowerInvokePass());
	Passes.add(createCFGSimplificationPass());	// clean up after lower invoke.
	Passes.add(FortyTwoWriter);
	pinavm::addGCInfoDeleter(Passes);

	Passes.run(*llvmMod);

	Out->flush();

	// Delete the raw_fd_ostream.
	if (Out != &fouts())
		delete Out;
}
