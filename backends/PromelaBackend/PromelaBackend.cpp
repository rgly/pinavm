#include "llvm/Support/raw_ostream.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Bitcode/ReaderWriter.h"
//#include "llvm/CodeGen/LinkAllAsmWriterComponents.h"
#include "llvm/CodeGen/LinkAllCodegenComponents.h"
//#include "llvm/CodeGen/ObjectCodeEmitter.h"
#include "llvm/Config/config.h"
#include "llvm/LinkAllVMCore.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileUtilities.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/PrettyStackTrace.h"
//#include "llvm/System/Host.h"
//#include "llvm/System/Signals.h"
//#include "llvm/Target/SubtargetFeature.h"
#include "llvm/DataLayout.h"
#include "llvm/Target/TargetMachine.h"
//#include "llvm/Target/TargetRegistry.h"
//#include "llvm/Target/TargetSelect.h"
#include "llvm/Transforms/Scalar.h"
//#include "llvm/Analysis/LoopDependenceAnalysis.h"

#include "Frontend.hpp"
#include "PromelaWriter.h"
#include "PromelaBackend.h"

using namespace llvm;

void launch_promelabackend(Frontend * fe,
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
		raw_fd_ostream *FDOut = new raw_fd_ostream(OutputFilename.c_str(), error);
		if (!error.empty()) {
			errs() << error << '\n';
			delete FDOut;
			return;
		}
		Out = new formatted_raw_ostream(*FDOut,	formatted_raw_ostream::DELETE_STREAM);

		// Make sure that the Output file gets unlinked from the disk if we get a
		// SIGINT
		sys::RemoveFileOnSignal(sys::Path(OutputFilename));
	}
	// Build up all of the passes that we want to do to the module.
	PassManager Passes;

	ModulePass *promelaWriter = new PromelaWriter(fe, *Out, useBoolInsteadOfInts, relativeClocks, bug);

	Passes.add(new DataLayout(llvmMod));
	Passes.add(createVerifierPass());
	Passes.add(createGCLoweringPass());
// MM: useless since free instruction became a normal function call.
//	Passes.add(createLowerAllocationsPass(true));
	Passes.add(createLowerInvokePass());
	Passes.add(createCFGSimplificationPass());	// clean up after lower invoke.
//	Passes.add(new PromelaBackendNameAllUsedStructsAndMergeFunctions());
	Passes.add(promelaWriter);
	Passes.add(createGCInfoDeleter());

	Passes.run(*llvmMod);

	Out->flush();

	// Delete the raw_fd_ostream.
	if (Out != &fouts())
		delete Out;
}
