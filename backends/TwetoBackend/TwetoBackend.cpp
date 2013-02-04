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

#include <llvm/LLVMContext.h>
#include <llvm/Type.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/CodeGen/LinkAllCodegenComponents.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/ExecutionEngine/JITEventListener.h>
#include <llvm/DataLayout.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/PrettyStackTrace.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IRBuilder.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/PassManager.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/Scalar.h>
#include <cerrno>

#include "llvm/ValueSymbolTable.h"
#include "llvm/Module.h"

#include "Frontend.hpp"
#include "TwetoBackend.h"
#include "TwetoSpecialize.h"
#include "TwetoConstMemory.h"
#include <systemc>

#include "TLMBasicPass.h"

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

static ExecutionEngine *EE = 0;
static IRBuilder<> *IRB = 0; 
static DataLayout *TD = NULL;
static Module *llvmMod = 0;
static PassManager *PM = NULL;
static bool optimizeProcess = true;
typedef std::pair<intptr_t, const Function *> IntFctPair;
static std::vector<IntFctPair> addr2function;


/**
 * tweto_optimize
 *
 */
static void tweto_optimize(Frontend * fe, ExecutionEngine *ee, 
                         sc_core::sc_simcontext* simcontext, 
                         const sc_core::sc_time& simduration,
                         bool disableMsg)
{
	
	llvmMod = fe->getLLVMModule();
    LLVMContext &Context = getGlobalContext();
    
    // Set the execution engine
    EE = ee;
    // Initialize the builder
    IRB = new IRBuilder<>(Context);    
    
    // Build up all of the passes that we want to do to the module.
    TD = new DataLayout(llvmMod);
	PM = new PassManager();
	// Defines target properties related to datatype  
	// size/offset/alignment information
	PM->add(TD); 
	// Check a module or function for validity
	/*PM->add(createVerifierPass()); 
	// Performs target-independent LLVM IR 
	// transformations for highly portable strategies
	PM->add(createGCLoweringPass()); 
	// @see Scalar.h : should not be used, not accurate in this case
	//PM->add(createLowerInvokePass()); 
	// Control flow graph simplification
	PM->add(createCFGSimplificationPass()); 
	// Releases GC metadata
	PM->add(createGCInfoDeleter()); 
	// Combine instructions to form fewer, simple instructions
	PM->add(createInstructionCombiningPass()); 
	// Reassociates commutative expressions in an order that is
	// designed to promote better constant propagation, GCSE, LICM, PRE...
	PM->add(createInstructionCombiningPass());
	/*PM->add(createReassociatePass());
	PM->add(createGVNPass());*/
    PM->add(new TLMBasicPass(fe, ee, disableMsg));
    
	// Execute all of the passes scheduled for execution
	PM->run(*llvmMod);
    
    // Sort the addresses-to-functions map
    std::sort(addr2function.begin(),addr2function.end());
    // Print specialized functions
    tweto_print_all_specialized_if_asked();
    
}

    
/**
 * launch_twetobackend
 *
 */
void launch_twetobackend(Frontend * fe, ExecutionEngine *ee, 
                         sc_core::sc_simcontext* simcontext, 
                         const sc_core::sc_time& simduration,
                         bool optimize, bool disablePrintMsg)
{
    if (optimize) {
        optimizeProcess = true;
        tweto_optimize(fe, ee, simcontext, simduration, disablePrintMsg);
    } else {
        optimizeProcess = false;
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
 * tweto_mark_const
 *
 */
void tweto_mark_const(const void *ptr_to_cst, size_t size) {
    //if (PrintMarkConst)
    //    std::cerr <<"tweto_mark_const: " <<std::dec <<(intptr_t)ptr_to_cst <<" (size " <<size <<").\n";
    add_interval(const_addresses,(intptr_t)ptr_to_cst,size);
}


/**
 * tweto_optimize_process
 *
 */
sc_core::SC_ENTRY_FUNC_OPT
tweto_optimize_process(sc_core::SC_ENTRY_FUNC vfct, sc_core::sc_process_host *host) {
    
    optimizeProcess = false;
    // Do not optimize 
    if(!optimizeProcess) {
        return NULL;
    }
    
    // using errs() in this function causes a crash when we use the option -stats
    formatted_raw_ostream *Out = &fouts();
    
    fill_const_addressses(llvmMod, EE);
    
    std::cout <<"tweto: begin process optimization\n";
    LLVMContext &Context = getGlobalContext();
    sc_core::sc_module *m = dynamic_cast<sc_core::sc_module*>(host);
    if (sc_core::sc_module *m = dynamic_cast<sc_core::sc_module*>(host)) {
        std::cerr <<"tweto: the process host is the module \"" <<m->name() <<"\".\n";
        add_interval(const_addresses,(intptr_t)m,sizeof(void*)); // vtable
        // std::cerr <<"tweto: vtable " <<*reinterpret_cast<intptr_t*>(m) <<'\n';
    } else {
        std::cerr <<"tweto: error: the process host is not a module (maybe a dynamic process).\n";
        abort();
    }
    assert(host);
    
    Function *call_proc = llvmMod->getFunction("tweto_call_process_method");
    if(call_proc==NULL) {
        std::cerr <<"tweto: error: tweto_call_process_method not found :\n";
        std::cerr <<"              use the 'run' option to disable optimizations.\n";
        exit(1);
    }
    
    call_proc->dump();
    
    FunctionType *call_proc_FT = call_proc->getFunctionType();
    IntegerType *i64 = Type::getInt64Ty(Context);
    IntegerType *i32 = Type::getInt32Ty(Context);
    
    // Enter machine-dependent code
    Value *args[3];
    // DO NOT WORK (ON MACOSX 10.6.6) : BUG ON FIRST ARGUMENT (i8 instead of i64)
    if (call_proc_FT->getNumParams()==3 &&
        call_proc_FT->getParamType(0)==i64 && 
        call_proc_FT->getParamType(1)==i64 &&
        sizeof(void*)==8) {
        // Case used on 64-bits Machine running Snow Leopard
        union {
            sc_core::SC_ENTRY_FUNC fct;
            int64_t nums[2];
        } vfct_conv;
        vfct_conv.fct = vfct;
        ConstantInt *vfct0 = ConstantInt::getSigned(i64,vfct_conv.nums[0]);
        args[0] = vfct0;
        ConstantInt *vfct1 = ConstantInt::getSigned(i64,vfct_conv.nums[1]);
        args[1] = vfct1;
        ConstantInt *host_int = ConstantInt::getSigned(i64,reinterpret_cast<intptr_t>(host));
        Type *host_type_ptr = call_proc_FT->getParamType(2);
        Value *host_ptr = IRB->CreateIntToPtr(host_int,host_type_ptr,"host_ptr");
        args[2] = host_ptr;
    } else if (call_proc_FT->getNumParams()==3 &&
               call_proc_FT->getParamType(1)==i32 &&
               sizeof(void*)==4) {
        // Case used on 32-bits Machine running Linux
        union {
            sc_core::SC_ENTRY_FUNC fct;
            int32_t nums[2];
        } vfct_conv;
        vfct_conv.fct = vfct;
        ConstantInt *vfct0 = ConstantInt::getSigned(i32,vfct_conv.nums[0]);
        Type *arg0_type = call_proc_FT->getParamType(0);
        args[0] = IRB->CreateIntToPtr(vfct0,arg0_type,"arg0");
        ConstantInt *vfct1 = ConstantInt::getSigned(i32,vfct_conv.nums[1]);
        args[1] = vfct1;
        ConstantInt *host_int = ConstantInt::getSigned(i32,reinterpret_cast<intptr_t>(host));
        Type *host_type_ptr = call_proc_FT->getParamType(2);
        Value *host_ptr = IRB->CreateIntToPtr(host_int,host_type_ptr,"host_ptr");
        args[2] = host_ptr;
    } else {
        // Other cases are not supported yet
        std::cerr <<"tweto: error: unsupported function type.\n";
        return NULL;
        //abort(); 
    }
    // End of machine-dependent code
    
    Function *F = tweto_specialize(EE,llvmMod,call_proc,args,args+3);
    assert(F);
    verifyModule(*llvmMod);
    PM->run(*llvmMod);
    
    void *FPtr = EE->getPointerToFunction(F);
    verifyModule(*llvmMod);
    // Dump
    //if (Dump)
    //    F->dump();
    // std::cerr <<"tweto: end process optimization\n";
    return reinterpret_cast<sc_core::SC_ENTRY_FUNC_OPT>(FPtr);
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
