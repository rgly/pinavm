/**
 * TwetoBackend.cpp
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : si-mohamed.lamraoui@imag.fr
 * @date : 2011/02/20
 * @copyright : Verimag 2011
 */

#include "llvm/Support/raw_ostream.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/CodeGen/LinkAllCodegenComponents.h"
#include "llvm/CodeGen/ObjectCodeEmitter.h"
#include "llvm/Config/config.h"
#include "llvm/LinkAllVMCore.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileUtilities.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/System/Host.h"
#include "llvm/System/Signals.h"
#include "llvm/Target/SubtargetFeature.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetRegistry.h"
#include "llvm/Target/TargetSelect.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Analysis/LoopDependenceAnalysis.h"

#include "Frontend.hpp"
#include "TwetoBackend.h"

using namespace llvm;

void launch_twetobackend(Frontend * fe,std::string OutputFilename,
						 bool useBoolInsteadOfInts,bool relativeClocks,bool bug)
{
	
	Module *llvmMod = fe->getLLVMModule();

	
	/** 
	 * Figure out what stream we are supposed to write to...
	 */
	formatted_raw_ostream *Out = &fouts(); // FIXME: outs() is not binary!
	if (OutputFilename != "-") {
		std::string error;
		raw_fd_ostream *FDOut = new raw_fd_ostream(OutputFilename.c_str(), error);
		if (!error.empty()) {
			errs() << error << '\n';
			delete FDOut;
			return;
		}
		Out = new formatted_raw_ostream(*FDOut,	formatted_raw_ostream::DELETE_STREAM);
		// Make sure that the Output file gets unlinked from 
		// the disk if we get a SIGINT
		sys::RemoveFileOnSignal(sys::Path(OutputFilename));
	}
	
	/** 
	 * Build up all of the passes that we want to do to the module.
	 */
	PassManager Passes;
	// Defines target properties related to datatype  
	// size/offset/alignment information
	Passes.add(new TargetData(llvmMod)); 
	// Check a module or function for validity
	Passes.add(createVerifierPass()); 
	// Performs target-independent LLVM IR 
	// transformations for highly portable strategies
	Passes.add(createGCLoweringPass()); 
	// @see Scalar.h : should not be used, not accurate in this case
	//Passes.add(createLowerInvokePass()); 
	// Control flow graph simplification
	Passes.add(createCFGSimplificationPass()); 
	// Releases GC metadata
	Passes.add(createGCInfoDeleter()); 
	// Combine instructions to form fewer, simple instructions
	Passes.add(createInstructionCombiningPass()); 
	// Reassociates commutative expressions in an order that is
	// designed to promote better constant propagation, GCSE, LICM, PRE...
	PM->add(createInstructionCombiningPass());
	/*PM->add(createReassociatePass());
	PM->add(createGVNPass());*/
	
	/**
	 * Execute all of the passes scheduled for execution
	 */
	Passes.run(*llvmMod);

	
	Out->flush();

	// Delete the raw_fd_ostream.
	if (Out != &fouts())
		delete Out;
}
