/**
 * TLMBasicPass.cpp
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : si-mohamed.lamraoui@imag.fr
 * @date : 2011/03/17
 * @copyright : Verimag 2011
 */

#include <iostream>
#include <string>
#include <fstream>
#include <map>

#include "llvm/GlobalValue.h"
#include "llvm/CallingConv.h"
#include "llvm/DerivedTypes.h"
#include "llvm/InstrTypes.h"
#include "llvm/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Target/Mangler.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/LLVMContext.h"
#include "llvm/Analysis/Verifier.h"
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/Support/InstIterator.h>
#include <llvm/Transforms/Utils/Cloning.h>
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Pass.h"
#include "llvm/PassManagers.h"
#include "llvm/Support/Debug.h"
#include "llvm/Function.h"

#include "Port.hpp"
#include "Channel.hpp"
#include "SimpleChannel.hpp"
#include "SCCFactory.hpp"
#include "SCElab.h"
#include "SCConstruct.hpp"
#include "EventConstruct.hpp"
#include "ReadConstruct.hpp"
#include "WriteConstruct.hpp"
#include "AssertConstruct.hpp"
#include "RandConstruct.hpp"
#include "Process.hpp"
#include "IRModule.hpp"
#include "SCJit.hpp"

#include "sysc/kernel/sc_process_table.h"
#include "sysc/kernel/sc_simcontext.h"
#include "sysc/kernel/sc_thread_process.h"
#include "sysc/kernel/sc_module_registry.h"
#include "sysc/kernel/sc_module.h"
#include "sysc/kernel/sc_process.h"
#include "sysc/communication/sc_port.h"

#include "basic.h"
#include "bus.h"

#include "TLMBasicPass.h"
#include "LoopChecker.h"

char TLMBasicPass::ID = 0;
const std::string wFunName = "_ZN5basic21initiator_socket_baseILb0EE5writeERKjji";
const std::string rFunName = "_ZN5basic21initiator_socket_baseXXXXXXXXXXXXXXXXXX";
int proc_counter = 0;


// =============================================================================
// TLMBasicPass 
// 
// Constructor
// 
// =============================================================================
TLMBasicPass::TLMBasicPass(Frontend *fe, ExecutionEngine *ee, 
                           bool disableMsg) : ModulePass(ID) {
    this->callOptCounter = 0;
    this->rwCallsCounter = 0;
    this->fe = fe; 
    this->engine = ee;
    this->disableMsg = disableMsg;
    this->elab = fe->getElab();
    // Checking machine's data size
    int n = sizeof(void *);
    if(n==8)
        this->is64Bit = true;
    else // n = 4
        this->is64Bit = false;
}


// =============================================================================
// runOnModule
// 
//  
// 
// =============================================================================
bool TLMBasicPass::runOnModule(Module &M) {
    
    MSG("\n============== TLM Basic Pass =============\n");
    this->llvmMod = &M;
    // Initialize function passes
    TargetData *target = new TargetData(this->llvmMod);
    funPassManager = new FunctionPassManager(this->llvmMod);
    funPassManager->add(target);
    funPassManager->add(createIndVarSimplifyPass());
    funPassManager->add(createLoopUnrollPass());
    funPassManager->add(createInstructionCombiningPass());
    funPassManager->add(createReassociatePass());
    funPassManager->add(createGVNPass());
    funPassManager->add(createCFGSimplificationPass());
    funPassManager->add(createConstantPropagationPass());
    
    // Modules
    vector < sc_core::sc_module * >modules =
    sc_core::sc_get_curr_simcontext()->get_module_registry()->m_module_vec;
	vector < sc_core::sc_module * >::iterator modIt;
	for (modIt = modules.begin(); modIt < modules.end(); ++modIt) {
		sc_core::sc_module * initiatorMod = *modIt;
        std::string moduleName = (std::string) initiatorMod->name();
        
        // Initiator ports
		std::vector < sc_core::sc_port_base * >*ports = initiatorMod->m_port_vec;
		vector < sc_core::sc_port_base * >::iterator it;
		for (it = ports->begin(); it < ports->end(); ++it) {
			sc_core::sc_port_base * initiator = *it;
            sc_core::sc_interface* initiatorItf = initiator->get_interface();
            
            std::string initiatorName = initiator->name(); 
            if (initiatorName.find("basic::initiator_socket")!=string::npos) {
                
                MSG("== Initiator : "+initiatorName+"\n");
                basic::target_socket_base<true> *tsb =
                dynamic_cast<basic::target_socket_base<true> *>(initiatorItf);
                if (tsb) {
                    Bus *b = dynamic_cast<Bus *>(tsb->get_parent_object());
                    for(int i = 0; i < b->initiator.size(); ++i) {
                        basic::compatible_socket* target =
                        dynamic_cast<basic::compatible_socket*>(b->initiator[i]);
                        if (target) {
                            std::string targetName = target->name();
                            MSG(" = Target : "+targetName+"\n");
                            
                            sc_core::sc_object *o = target->get_parent();
                            sc_core::sc_module *targetMod = 
                            dynamic_cast<sc_core::sc_module *>(o);
                            
                            optimize(target, initiatorMod, targetMod, b); 
                            
                        }
                    }
                
                }                
                
            }
		}
	}

    // Check if the module is corrupt
    verifyModule(*this->llvmMod);
    std::ostringstream oss;
    oss << callOptCounter;
    MSG("\n Pass report - "+oss.str()+"/"+"?");
    MSG(" - opt/total\n");
    MSG("===========================================\n\n");
    
    // TODO : handle false case
    return true;
}


// =============================================================================
// optimize
// 
// Optimize all process that use write or read functions
// =============================================================================
void TLMBasicPass::optimize(basic::compatible_socket* target, 
                            sc_core::sc_module *initiatorMod, 
                            sc_core::sc_module *targetMod, 
                            Bus *bus) {
    std::ostringstream oss;
    // Looking for declarations of functions
    Function *writef = lookForWriteFunction(targetMod);
    Function *readf = lookForReadFunction(targetMod);
    if (writef==NULL && readf==NULL)
        return;
    
    // Looking for calls in process
    std::vector<std::string> doneThreads;
    std::vector<std::string> doneMethods;
    vector<std::string>::iterator it;
    std::string fctName;
    sc_core::sc_process_table * processes = 
    initiatorMod->sc_get_curr_simcontext()->m_process_table;
    sc_core::sc_thread_handle thread_p;
    for (thread_p = processes->thread_q_head(); 
         thread_p; thread_p = thread_p->next_exist()) {
        sc_core::sc_process_b *proc = thread_p;
        // Test if the thread's function has already 
        // been optimized for this target
        fctName = proc->func_process;
        it = find (doneThreads.begin(), doneThreads.end(), fctName);
        if (it==doneThreads.end()) {
            doneThreads.push_back(fctName);
            replaceCallsInProcess(target, initiatorMod, targetMod,
                                  proc, writef, readf, bus);
        }
    }
    sc_core::sc_method_handle method_p;
    for (method_p = processes->method_q_head(); 
         method_p; method_p = method_p->next_exist()) {
        //sc_core::sc_method_process *proc = method_p;
        sc_core::sc_process_b *proc = method_p;
        // Test if the method's function has already 
        // been optimized for this target
        fctName = proc->func_process;
        it = find (doneMethods.begin(), doneMethods.end(), fctName);
        if (it==doneMethods.end()) {
            doneMethods.push_back(fctName);
            replaceCallsInProcess(target, initiatorMod, targetMod, 
                                  proc, writef, readf, bus);
        }
    }
    
}


// =============================================================================
// replaceCallsInProcess
// 
// Replace indirect calls to write() or read() by direct calls 
// in the given process.
// =============================================================================
void TLMBasicPass::replaceCallsInProcess(basic::compatible_socket* target, 
                                         sc_core::sc_module *initiatorMod, 
                                         sc_core::sc_module *targetMod,
                                         sc_core::sc_process_b *proc,
                                         Function *writef, Function *readf, 
                                         Bus *bus) {
    // Get associate function
    std::string fctName = proc->func_process;
	std::string modType = typeid(*initiatorMod).name();
	std::string mainFctName = "_ZN" + modType + 
    utostr(fctName.size()) + fctName + "Ev";
	Function *oldProcf = this->llvmMod->getFunction(mainFctName);
    if (oldProcf==NULL)
        return;
    
    // We do not modifie the original function
    // Instead, we create a clone.
    Function *procf = createProcess(oldProcf, initiatorMod);
    void *funPtr = this->engine->getPointerToFunction(procf); 
    sc_core::SC_ENTRY_FUNC_OPT scfun = 
    reinterpret_cast<sc_core::SC_ENTRY_FUNC_OPT>(funPtr);
    proc->m_semantics_p = scfun;
    std::string procfName = procf->getName();
    MSG("      Replace in the process's function : "+procfName+"\n");
    
    std::ostringstream oss;
    std::vector<CallInfo*> *work = new std::vector<CallInfo*>;
    
    inst_iterator ii;
    for (ii = inst_begin(procf); ii!=inst_end(procf); ii++) {
        Instruction &i = *ii;
        CallSite cs = CallSite::get(&i);
        if (cs.getInstruction()) {
            // Candidate for a replacement
            Function *oldfun = cs.getCalledFunction();
            if (oldfun!=NULL && !oldfun->isDeclaration()) {
                std::string name = oldfun->getName();
                // === Write ===
                if (writef!=NULL && !strcmp(name.c_str(), wFunName.c_str())) {
                    
                    CallInfo *info = new CallInfo();
                    info->oldcall = dyn_cast<CallInst>(cs.getInstruction());
                    MSG("       Checking adress : ");
                    // Retrieve the argument by executing 
                    // the appropriated piece of code
                    SCJit *scjit = new SCJit(this->llvmMod, this->elab);
                    Process *irProc = this->elab->getProcess(proc);
                    scjit->setCurrentProcess(irProc);                    
                    bool jitErr = false;
                    info->addrArg = cs.getArgument(1);
                    int value = 
                    scjit->jitInt(procf, info->oldcall, info->addrArg, &jitErr);
                    if(jitErr) {
                        std::cout << "       cannot get the address value!" 
                          << std::endl;
                    } else {
                    oss.str("");  oss << std::hex << value;
                    MSG("0x"+oss.str()+"\n");
                    basic::addr_t a = static_cast<basic::addr_t>(value);            
                    
                    // Checking address and target concordance
                    bool concordErr = bus->checkAdressConcordance(target, a);
                    if(!concordErr) {
                        std::cout << "       return, no concordances!"
                        << std::endl;
                    } 
                    else {
                    
                    // Checking address alignment
                    if(value % sizeof(basic::data_t)) {
                        std::cerr << "  unaligned write : " <<
                        std::hex << value << std::endl;
                        abort();
                    }
                    
                    // Checking address range
                    bool addrErr = bus->checkAdressRange(a);
                    if(!addrErr) {
                        std::cerr << "  no target at address " <<
                        std::hex << value << std::endl;
                        abort();
                    }
                    
                    // Save informations to build a new call later
                    const FunctionType *writeFunType = writef->getFunctionType();  
                    info->targetType = writeFunType->getParamType(0);
                    LLVMContext &context = getGlobalContext();
                    const IntegerType *intType;
                    if (this->is64Bit) {
                        intType = Type::getInt64Ty(context);
                    } else {
                        intType = Type::getInt32Ty(context);
                    }
                    info->targetModVal = ConstantInt::getSigned(intType,
                                        reinterpret_cast<intptr_t>(targetMod));
                    info->dataArg = cs.getArgument(2);
                    work->push_back(info);
                    }
                    }
   
                } else
                    
                // === Read ===
                if (readf!=NULL && !strcmp(name.c_str(), rFunName.c_str())) {
                    
                    // Not yet supported
                                        
                }
            }  
        }
    
    }
        
    // Before
    //procf->dump();
    
    // Replace calls
    std::vector<CallInfo*>::iterator it;
    for (it = work->begin(); it!=work->end(); ++it) {
        CallInfo *i = *it;
        
        // Retrieve a pointer to target module 
        //  %1 = module
        //  %2 = inttoptr i64/i32 %1 to %struct.target*
        IntToPtrInst *structTarget = 
        new IntToPtrInst(i->targetModVal, i->targetType, "", i->oldcall);
        
        // Get a pointer to the data argument
        //  %6 = alloca i32
        //  store i32 %data, i32* %6	
        BasicBlock &front = procf->getEntryBlock();
        Instruction *nophi = front.getFirstNonPHI();
        if(nophi==NULL) {
            std::cerr << "  Code insertion error [PHI inst]\n";
            abort();
        }
        AllocaInst* dataArgPtr = 
        new AllocaInst(i->dataArg->getType(), 0, "", nophi);
        dataArgPtr->setAlignment(4);
        StoreInst *storeData = 
        new StoreInst(i->dataArg, dataArgPtr, i->oldcall);
        storeData->setAlignment(4);
        
        // Set arguments
        Value **argsKeep = new Value*[3];
        Value **argsKeepEnd = argsKeep;
        *argsKeepEnd++ = structTarget; // struct.target*
        *argsKeepEnd++ = i->addrArg; // addr_t*
        *argsKeepEnd++ = dataArgPtr; // data_t*
        
        // Create a new call
        i->newcall = CallInst::Create(writef, argsKeep,argsKeepEnd,"");

        // Replace
        BasicBlock::iterator it(i->oldcall);
        ReplaceInstWithInst(i->oldcall->getParent()->getInstList(), it, i->newcall);
        i->oldcall->replaceAllUsesWith(i->newcall);
        
        MSG("       Call optimized (^_-)\n");
        callOptCounter++;
    }
    
    //std::cout << "==================================\n";
    // Run preloaded passes on the function to propagate constants
    funPassManager->run(*procf);
    // After
    //procf->dump();        
    // Check if the function is corrupt
    verifyFunction(*procf);
    this->engine->recompileAndRelinkFunction(procf);
}


// =============================================================================
// createProcess
// 
// Create a new function that contains a call to the old function.
// We inline the call in order to clone the old function's implementation.
// =============================================================================
Function *TLMBasicPass::createProcess(Function *oldProc, 
                                      sc_core::sc_module *initiatorMod) {
    
    LLVMContext &context = getGlobalContext();
    const IntegerType *intType;
    if (this->is64Bit) {
        intType = Type::getInt64Ty(context);
    } else {
        intType = Type::getInt32Ty(context);
    }
    
    // Retrieve a pointer to the initiator module 
    ConstantInt *initiatorModVal = 
    ConstantInt::getSigned(intType,reinterpret_cast<intptr_t>(initiatorMod));
    const FunctionType *funType = oldProc->getFunctionType();  
    const Type *type = funType->getParamType(0);
    IntToPtrInst *thisAddr = 
    new IntToPtrInst(initiatorModVal, type, "");
    
    // Compute the type of the new function
    const FunctionType *oldProcType = oldProc->getFunctionType();
    Value **argsBegin = new Value*[1];
    Value **argsEnd = argsBegin;
    *argsEnd++ = thisAddr;
    const unsigned argsSize = argsEnd-argsBegin;
    Value **args = argsBegin;
    assert(oldProcType->getNumParams()==argsSize);
    assert(!oldProc->isDeclaration());
    std::vector<const Type*> argTypes;
    for (unsigned i = 0; i!=argsSize; ++i)
        //if (args[i]==NULL)
            argTypes.push_back(oldProcType->getParamType(i));
    const FunctionType *newProcType =
    FunctionType::get(oldProc->getReturnType(), argTypes, false);
    
    // Create the new function
    std::ostringstream id;
    id << proc_counter++;
    std::string name = oldProc->getNameStr()+std::string("_clone_")+id.str();
    Function *newProc = 
    Function::Create(newProcType, Function::ExternalLinkage, name, this->llvmMod);
    assert(newProc->empty());
    newProc->addFnAttr(Attribute::InlineHint);
    
    { // Set name of newfunc arguments and complete args
        Function::arg_iterator nai = newProc->arg_begin();
        Function::arg_iterator oai = oldProc->arg_begin();
        for (unsigned i = 0; i!=argsSize; ++i, ++oai)
            if (args[i]==NULL) {
                nai->setName(oai->getName());
                args[i] = nai;
                ++nai;
            }
        assert(nai==newProc->arg_end());
        assert(oai==oldProc->arg_end());
    }
    
    // Create call to old function
    BasicBlock *bb = BasicBlock::Create(context, "entry", newProc);
    IRBuilder<> *irb = new IRBuilder<>(context);
    irb->SetInsertPoint(bb);
    CallInst *ci = irb->CreateCall(oldProc, argsBegin, argsEnd);
    bb->getInstList().insert(ci, thisAddr);
    if (ci->getType()->isVoidTy())
        irb->CreateRetVoid();
    else
        irb->CreateRet(ci);

    // The function should be valid now
    verifyFunction(*newProc);
    
    { // Inline the call
        TargetData *td = new TargetData(this->llvmMod);
        InlineFunctionInfo i(NULL, td);
        bool success = InlineFunction(ci, i);
        assert(success);
        verifyFunction(*newProc);
    }    
    
    //newProc->dump();
    return newProc;
}


// =============================================================================
// lookForWriteFunction
// 
// Look for a write function definition 
// in the given SystemC module
// =============================================================================
Function* TLMBasicPass::lookForWriteFunction(sc_core::sc_module *module) {
    // Reverse mangling
    std::string moduleType = typeid(*module).name();    
    std::string name("_ZN"+moduleType+"5writeERKjS1_");
    Function *f = this->llvmMod->getFunction(name);
    if(f!=NULL) {
        std::string moduleName = (std::string) module->name();
        MSG("      Found 'write' function in the module : "); 
        MSG(moduleName+"\n");
    }
    return f;
}


// =============================================================================
// lookForWriteFunction
// 
// Look for a read function definition 
// in the given SystemC module
// =============================================================================
Function* TLMBasicPass::lookForReadFunction(sc_core::sc_module *module) {
    // Reverse mangling
    std::string moduleType = typeid(*module).name();
    std::string name("_ZN"+moduleType+"4readERKjRj");    
    Function *f = this->llvmMod->getFunction(name);
    if(f!=NULL) {
        std::string moduleName = (std::string) module->name();
        MSG("      Found 'read' function in the module : "); 
        MSG(moduleName+"\n");
    }
    return f;
}


// =============================================================================
// MSG
// 
// Print the given message 
// if the user did not use -dis-opt-msg
// =============================================================================
void TLMBasicPass::MSG(std::string msg) {
    if(!this->disableMsg)
        std::cout << msg;
}

