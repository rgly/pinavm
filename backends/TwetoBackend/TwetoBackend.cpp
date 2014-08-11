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
#include <llvm/IR/Verifier.h>
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

static Module *llvmMod = 0;
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
    
	// Build up all of the passes that we want to do to the module.
	DataLayout* TD = new DataLayout(llvmMod);
	PassManager* PM = new PassManager();
	// Defines target properties related to datatype  
	// size/offset/alignment information

	// To compatiable with legacy passmanager. DataLayout is not
	// a pass anymore.
	DataLayoutPass* dlpass = new DataLayoutPass(*TD);

	PM->add(dlpass);
	PM->add(createPromoteMemoryToRegisterPass());
	PM->add(new TwetoPass(fe, NULL, optlevel, disableMsg));
    
	// initialize the llvm global stack_base variable
	sbase = new GlobalVariable (*llvmMod,
			Type::getInt8PtrTy(Context), false,
			GlobalValue::ExternalLinkage, NULL, "stack_base",
			0, GlobalVariable::NotThreadLocal, 0, true);

	// Execute all of the passes scheduled for execution
	PM->run(*llvmMod);
	delete PM;
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
			// build main() function
			{
				LLVMContext& c = llvmMod->getContext();
				// size to be allocated in gv
				// TODO: allow user to modify it
				size_t memsize = 0x100000;
				// ext void permalloc::placement_init(void*,size_t)
				std::vector<Type*> permargs;
				permargs.push_back
					(Type::getInt8Ty(c)->getPointerTo());
				permargs.push_back
					(Type::getIntNTy(c,CHAR_BIT*sizeof(void*)));
				FunctionType* permtype = FunctionType::get
					(Type::getVoidTy(c), permargs, false);
				Function* perminit = Function::Create
					(permtype, GlobalValue::ExternalLinkage,
					 "_ZN9permalloc14placement_initEPvm",
					 llvmMod);
				// ext void launch_systemc (int, char**)
				std::vector<Type*> mainargs;
				// assumes ilp32, lp64 or llp64
				mainargs.push_back (Type::getInt32Ty(c));
				mainargs.push_back (
						Type::getInt8Ty(c)->getPointerTo()
						                  ->getPointerTo());
				FunctionType* maintype = FunctionType::get
					(Type::getInt32Ty(c), mainargs, false);
				Function* launch_systemc = Function::Create
					(maintype, GlobalValue::ExternalLinkage,
					 "_Z14launch_systemciPPc", llvmMod);
				// char array[memsize]
				ArrayType* memtype = ArrayType::get
					(Type::getInt8Ty(c), memsize);
				GlobalVariable* memgv = new GlobalVariable
					(*llvmMod, memtype, false,
					 GlobalValue::ExternalLinkage,
					 0, "array");
				// allocate the array
				ConstantAggregateZero* memcaz =
					ConstantAggregateZero::get (memtype);
				memgv->setInitializer (memcaz);
				// define main (int, char**)
				Function* mainfunc = Function::Create
					(maintype, Function::ExternalLinkage,
					 "main", llvmMod);
				BasicBlock* bb = BasicBlock::Create
					(c, "entry", mainfunc);
				IRBuilder<> irb (c);
				irb.SetInsertPoint (bb);
				// call permalloc::placement_init
				std::vector<Value*> piargs;
				ConstantInt* zero = ConstantInt::get
					(c, APInt (64, StringRef("0"), 10));
				std::vector<Constant*> memindices(2, zero);
				Constant* memptr = ConstantExpr::getGetElementPtr
					(memgv, memindices);
				piargs.push_back (memptr);
				piargs.push_back (ConstantInt::get
						(c,
						 APInt (CHAR_BIT*sizeof(void*),
							StringRef("100000"),
						 	16)));
				CallInst* picall = irb.CreateCall (perminit,piargs);
				// picall returns void
				(void) picall;
				// call launch_systemc
				std::vector<Value*> lscargs;
				{
					// retrieve main() arguments
					Function::arg_iterator args =
						mainfunc->arg_begin();
					Value* i32_argc = args++;
					Value* ppi8_argv = args++;
					lscargs.push_back (i32_argc);
					lscargs.push_back (ppi8_argv);
				}
				CallInst* lsccall = irb.CreateCall
					(launch_systemc, lscargs);
				irb.CreateRet(lsccall);
			}
			break;
		case dynopt:
			MethodRelinker (ee).relinkEverything();
			break;
		default:
			break;
	}

	if (optimize == staticopt) {
		// output resulting module if optimizing statically
		std::string errorinfo;
		llvm::raw_fd_ostream llfd ("output.bc", errorinfo, None);
		WriteBitcodeToFile (fe->getLLVMModule(), llfd);
	} else {
		// otherwise, launch simulation
		if(!disablePrintMsg) {
			std::cout << "########### Launching simulation ############\n"; 
			std::cout.flush();
		}
		assert(simcontext);
		simcontext->simulate(simduration);
	}
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

