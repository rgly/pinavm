/**
 * TwetoBackend.cpp
 *
 * 
 *
 * @author : Claude Helmstetter, Si-Mohamed Lamraoui
 * @contact : si-mohamed.lamraoui@imag.fr
 * @date : 2011/02/20
 * @copyright : Verimag 2011
 */

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/CodeGen/LinkAllCodegenComponents.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/ExecutionEngine/JITEventListener.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/PrettyStackTrace.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/PassManager.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/ADT/StringExtras.h>
#include <llvm/Bitcode/BitstreamWriter.h>
#include <cerrno>

#include "llvm/IR/ValueSymbolTable.h"
#include "llvm/IR/Module.h"

#include "Frontend.hpp"
#include "TwetoBackend.h"
#include <systemc>
#include "sysc/kernel/sc_process_table.h"
#include "sysc/kernel/sc_simcontext.h"
#include "sysc/kernel/sc_thread_process.h"
#include "sysc/kernel/sc_module_registry.h"
#include "sysc/kernel/sc_module.h"
#include "sysc/kernel/sc_process.h"
#include "sysc/pinavm/permalloc.h"

#include "TwetoPass.h"
#include "MethodRelinker.h"

using namespace llvm;

/*namespace {
    cl::opt<bool>
    Dump("dump",
         cl::desc("Dump the specialized processes"),
         cl::init(false));
    
    cl::opt<bool>
    PrintMarkConst("print-mark-const",
                   cl::desc("Print the addresses which are marked as const"),
                   cl::init(false));
}*/

static IRBuilder<> *IRB = 0; 
static DataLayout *TD = NULL;
static Module *llvmMod = 0;
static PassManager *PM = NULL;
typedef std::pair<intptr_t, const Function *> IntFctPair;
static std::vector<IntFctPair> addr2function;
GlobalVariable* sbase;


/**
 * tweto_optimize
 *
 */
static void tweto_optimize(Frontend * fe,
                         sc_core::sc_simcontext* simcontext, 
                         const sc_core::sc_time& simduration,
			 enum tweto_opt_level optlevel,
                         bool disableMsg)
{
	
	llvmMod = fe->getLLVMModule();
	LLVMContext &Context = getGlobalContext();
    
	// Initialize the builder
	IRB = new IRBuilder<>(Context);    

	// Build up all of the passes that we want to do to the module.
	TD = new DataLayout(llvmMod);
	PM = new PassManager();
	// Defines target properties related to datatype  
	// size/offset/alignment information
	PM->add(TD); 
	PM->add(new TwetoPass(fe, NULL, optlevel, disableMsg));
    
	// initialize the llvm global stack_base variable
	sbase = new GlobalVariable (*llvmMod,
			IRB->getInt8PtrTy(), false,
			GlobalValue::ExternalLinkage, NULL, "stack_base",
			0, GlobalVariable::NotThreadLocal, 0, true);

	// Execute all of the passes scheduled for execution
	PM->run(*llvmMod);
}

void launch_twetobackend(Frontend * fe,
			BackendOption& option)
{
    launch_twetobackend(fe, option.context, *(option.duration), dynopt, option.DisableOptDbgMsg);
}

void launch_runbackend(Frontend * fe,
			BackendOption& option)
{
    launch_twetobackend(fe, option.context, *(option.duration), noopt, option.DisableOptDbgMsg);
}
 
void launch_staticbackend(Frontend * fe,
			BackendOption& option)
{
    launch_twetobackend(fe, option.context, *(option.duration), staticopt, option.DisableOptDbgMsg);
}
    
/**
 * launch_twetobackend
 *
 */
void launch_twetobackend(Frontend * fe, 
                         sc_core::sc_simcontext* simcontext, 
                         const sc_core::sc_time& simduration,
                         enum tweto_opt_level optimize, bool disablePrintMsg)
{
	// optimize the llvm ir (if run with -b tweto)
	if (optimize > noopt) {
		tweto_optimize(fe, simcontext, simduration, optimize, disablePrintMsg);
	}

	// Rebuild another execution engine
	llvmMod = fe->getLLVMModule();
	EngineBuilder builder(llvmMod);
	std::string ErrorMsg;
	builder.setErrorStr(&ErrorMsg);
	builder.setEngineKind(EngineKind::JIT);
	builder.setUseMCJIT(true);
	builder.setOptLevel(CodeGenOpt::None);
	ExecutionEngine* ee = builder.create();
	if (!ee) {
		if (!ErrorMsg.empty())
			errs() << "error creating 2nd EE: " << ErrorMsg << "\n";
		else
			errs() << "unknown error creating 2nd EE!\n";
		exit(1);
	}

	// prepare the 2nd exec engine to run
	ee->finalizeObject();

	switch (optimize) {
		case staticopt:
			StaticMethodRelinker (fe->getLLVMModule(), ee).relinkEverything();
			break;
		case dynopt:
			MethodRelinker (ee).relinkEverything();
			break;
		default:
			break;
	}

	// output resulting module if optimizing statically
	if (optimize == staticopt) {
		std::string errorinfo;
		llvm::raw_fd_ostream llfd ("output.bc", errorinfo, None);
		WriteBitcodeToFile (fe->getLLVMModule(), llfd);
	}


	/**
	 * Launching simulation
	 */    
	if(!disablePrintMsg) {
		std::cout << "########### Launching simulation ############\n"; 
		std::cout.flush();
	}
	assert(simcontext);
	simcontext->simulate(simduration);
}

/**
 * dump_function
 *
 */
bool dump_function(const char *str) {
    Function *f = llvmMod->getFunction(str);
    if (f) {
        f->dump();
        return true;
    } else {
        return false;
    }
}

