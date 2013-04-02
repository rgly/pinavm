/**
 * TLMBasicPassImpl.cpp
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
#include <llvm/IRBuilder.h>
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
#include "SCElab.h"
#include "Process.hpp"
#include "IRModule.hpp"
#include "SCJit.hpp"
#include "ALLConstruct.h"
#include "SCCFactory.hpp"

#include "LinkExternalBitcode.h"

#include "sysc/kernel/sc_process_table.h"
#include "sysc/kernel/sc_simcontext.h"
#include "sysc/kernel/sc_thread_process.h"
#include "sysc/kernel/sc_module_registry.h"
#include "sysc/kernel/sc_module.h"
#include "sysc/kernel/sc_process.h"
#include "sysc/communication/sc_port.h"

#include "basic.h"
#include "bus.h"

#include "TLMBasicPassImpl.h"
#include "LoopChecker.h"

const std::string wFunName = "_ZN5basic21initiator_socket_baseILb0EE5writeERKjji";
const std::string rFunName = "_ZN5basic21initiator_socket_baseXXXXXXXXXXXXXXXXXX";
int proc_counter = 0;


// =============================================================================
// TLMBasicPassImpl 
// 
// Constructor
// 
// =============================================================================
TLMBasicPassImpl::TLMBasicPassImpl(Frontend *fe, ExecutionEngine *ee, 
                           bool disableMsg) {
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
bool TLMBasicPassImpl::runOnModule(Module &M) {
    
    MSG("\n============== TLM Basic Pass =============\n");
    this->llvmMod = &M;

    this->llvmMod = LinkExternalBitcode(this->llvmMod,
         "backends/TwetoBackend/tweto_call_method.ll");
    
    // Retrieve the method that does all the vtable calculations
    // in order to call the actual 'write' method (see replaceCallsInProcess)
    this->writeFun = this->llvmMod->getFunction("tweto_call_write_method");
    if(!this->writeFun) {
        std::cerr << "tweto_call_write_method is missing,"; 
        std::cerr << "pass aborded!" << std::endl;
        return false;
    }
    // Retrieve the base write method in order to get the types
    // of the 'this' pointer, the address and the data
    // (should be always mangled like this, so if the Basic-protocol's includes
    //  are correctly imported, no error will be throw)
    this->basicWriteFun = this->llvmMod->getFunction(wFunName);
    if(!this->basicWriteFun) {
        std::cerr << "basic's write method is missing,"; 
        std::cerr << "pass aborded!" << std::endl;
        return false;
    }
    
    writeFun->dump();    
    
    // Initialize function passes
    DataLayout *target = new DataLayout(this->llvmMod);
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
    std::vector < sc_core::sc_module * >modules =
    sc_core::sc_get_curr_simcontext()->get_module_registry()->m_module_vec;
	std::vector < sc_core::sc_module * >::iterator modIt;
	for (modIt = modules.begin(); modIt < modules.end(); ++modIt) {
		sc_core::sc_module * initiatorMod = *modIt;
        std::string moduleName = (std::string) initiatorMod->name();
        // Does the module have ports
        std::vector < sc_core::sc_port_base * > *ports = 
        initiatorMod->m_port_vec;
        if(ports->size()>0) {
            // Optimize this module
            optimize(initiatorMod); 
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
void TLMBasicPassImpl::optimize(sc_core::sc_module *initiatorMod) {
    
    std::ostringstream oss;
    // Looking for calls in process
    std::vector<std::string> doneThreads;
    std::vector<std::string> doneMethods;
    std::vector<std::string>::iterator it;
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
            replaceCallsInProcess(initiatorMod, proc);
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
            replaceCallsInProcess(initiatorMod, proc);
        }
    }
    
}


// =============================================================================
// replaceCallsInProcess
// 
// Replace indirect calls to write() or read() by direct calls 
// in the given process.
// =============================================================================
void TLMBasicPassImpl::replaceCallsInProcess(sc_core::sc_module *initiatorMod,
                                         sc_core::sc_process_b *proc) {
    
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
    sc_core::sc_module *targetMod;
    std::vector<CallInfo*> *work = new std::vector<CallInfo*>;
    
    inst_iterator ii;
    for (ii = inst_begin(procf); ii!=inst_end(procf); ii++) {
        Instruction &i = *ii;
        CallSite cs(&i);
        if (cs.getInstruction()) {
            // Candidate for a replacement
            Function *oldfun = cs.getCalledFunction();
            if (oldfun!=NULL && !oldfun->isDeclaration()) {
                std::string name = oldfun->getName();
                // === Write ===
                if (!strcmp(name.c_str(), wFunName.c_str())) {
                    
                    CallInfo *info = new CallInfo();
                    info->oldcall = dyn_cast<CallInst>(cs.getInstruction());
                    MSG("       Checking adress : ");
                    // Retrieve the adress argument by executing 
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
                    
                    // Checking address alignment
                    if(value % sizeof(basic::data_t)) {
                        std::cerr << "  unaligned write : " <<
                        std::hex << value << std::endl;
                        abort();
                    }

                    // Retreive the target module using the address
                    targetMod =  getTargetModule(initiatorMod, a);
                                    
                    // Save informations to build a new call later
                    FunctionType *writeFunType = 
                        this->basicWriteFun->getFunctionType();  
                    info->targetType = writeFunType->getParamType(0);
                    LLVMContext &context = getGlobalContext();
                    IntegerType *intType;
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
   
                } else
                    
                // === Read ===
                if (!strcmp(name.c_str(), rFunName.c_str())) {
                    
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
        
        LLVMContext &context = getGlobalContext();
        FunctionType *writeFunType = 
        this->writeFun->getFunctionType();
        IntegerType *i64 = Type::getInt64Ty(context);
        // Get a pointer to the target module
        basic::target_module_base *tmb = 
        dynamic_cast<basic::target_module_base*>(targetMod);
        Value *ptr = 
        ConstantInt::getSigned(i64, reinterpret_cast<intptr_t>(tmb));
        IntToPtrInst *modPtr = new IntToPtrInst(ptr, 
                                                writeFunType->getParamType(0),
                                                "myitp", i->oldcall);
        // Get a the address value
        LoadInst *addr = new LoadInst(i->addrArg, "", i->oldcall);
        
        // Create the new call
        Value *args[] = {modPtr, addr, i->dataArg};
        i->newcall = CallInst::Create(this->writeFun, ArrayRef<Value*>(args, 3));
        
        // Replace the old call
        BasicBlock::iterator it(i->oldcall);
        ReplaceInstWithInst(i->oldcall->getParent()->getInstList(), it, i->newcall);
        i->oldcall->replaceAllUsesWith(i->newcall);
        
        // Inline the new call
        DataLayout *td = new DataLayout(this->llvmMod);
        InlineFunctionInfo ifi(NULL, td);
        bool success = InlineFunction(i->newcall, ifi);
        if(!success) {
            MSG("       The call cannot be inlined (it's not an error :D)");
        }
        
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
// getTargetModule
// 
// Retreive a module that is in the range of the given address.
// Of course, this module and the initiator module (arg) must be 
// connected to the same bus.
// =============================================================================
sc_core::sc_module *TLMBasicPassImpl::getTargetModule(
                                    sc_core::sc_module *initiatorMod,
                                    basic::addr_t a) {
    
    std::vector < sc_core::sc_port_base * >*ports = initiatorMod->m_port_vec;
    std::vector < sc_core::sc_port_base * >::iterator it;
    for (it = ports->begin(); it < ports->end(); ++it) {
        sc_core::sc_port_base * initiator = *it;
        sc_core::sc_interface* initiatorItf = initiator->get_interface();
        std::string initiatorName = initiator->name(); 
        if (initiatorName.find("basic::initiator_socket")!=std::string::npos) {
            basic::target_socket_base<true> *tsb =
            dynamic_cast<basic::target_socket_base<true> *>(initiatorItf);
            if (tsb) {
                Bus *b = dynamic_cast<Bus *>(tsb->get_parent_object());
                for(int i = 0; i < b->initiator.size(); ++i) {
                    basic::compatible_socket* target =
                    dynamic_cast<basic::compatible_socket*>(b->initiator[i]);
                    if (target) {
                        bool concordErr = b->checkAdressConcordance(target, a);
                        if(concordErr) {
                                                
                            std::string targetName = target->name();
                            //MSG(" = "+initiatorName+" -> "+targetName+"\n");

                            sc_core::sc_object *o = target->get_parent();
                            sc_core::sc_module *targetMod = 
                            dynamic_cast<sc_core::sc_module *>(o);
                            return targetMod;
                        }
                    }
                }
            }                
        }
    }
    return NULL;
}


// =============================================================================
// createProcess
// 
// Create a new function that contains a call to the old function.
// We inline the call in order to clone the old function's implementation.
// =============================================================================
Function *TLMBasicPassImpl::createProcess(Function *oldProc, 
                                      sc_core::sc_module *initiatorMod) {
    
    LLVMContext &context = getGlobalContext();
    IntegerType *intType;
    if (this->is64Bit) {
        intType = Type::getInt64Ty(context);
    } else {
        intType = Type::getInt32Ty(context);
    }
    
    // Retrieve a pointer to the initiator module 
    ConstantInt *initiatorModVal = 
    ConstantInt::getSigned(intType,reinterpret_cast<intptr_t>(initiatorMod));
    FunctionType *funType = oldProc->getFunctionType();  
    Type *type = funType->getParamType(0);
    IntToPtrInst *thisAddr = 
    new IntToPtrInst(initiatorModVal, type, "");
    
    // Compute the type of the new function
    FunctionType *oldProcType = oldProc->getFunctionType();
    Value **argsBegin = new Value*[1];
    Value **argsEnd = argsBegin;
    *argsEnd++ = thisAddr;
    const unsigned argsSize = argsEnd-argsBegin;
    Value **args = argsBegin;
    assert(oldProcType->getNumParams()==argsSize);
    assert(!oldProc->isDeclaration());
    std::vector<Type*> argTypes;
    for (unsigned i = 0; i!=argsSize; ++i)
            argTypes.push_back(oldProcType->getParamType(i));
    FunctionType *newProcType =
    FunctionType::get(oldProc->getReturnType(), ArrayRef<Type*>(argTypes), false);
    
    // Create the new function
    std::ostringstream id;
    id << proc_counter++;
    std::string name = oldProc->getName().str()+std::string("_clone_")+id.str();
    Function *newProc = 
    Function::Create(newProcType, Function::ExternalLinkage, name, this->llvmMod);
    assert(newProc->empty());
    newProc->addFnAttr(Attributes::InlineHint);
    
    { // Set name of newfunc arguments and complete args
        Function::arg_iterator nai = newProc->arg_begin();
        Function::arg_iterator oai = oldProc->arg_begin();
        for (unsigned i = 0; i!=argsSize; ++i, ++oai) {
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
    CallInst *ci = irb->CreateCall(oldProc, ArrayRef<Value*>(argsBegin, argsEnd));
    bb->getInstList().insert(ci, thisAddr);
    if (ci->getType()->isVoidTy())
        irb->CreateRetVoid();
    else
        irb->CreateRet(ci);

    // The function should be valid now
    verifyFunction(*newProc);
    
    { // Inline the call
        DataLayout *td = new DataLayout(this->llvmMod);
        InlineFunctionInfo i(NULL, td);
        bool success = InlineFunction(ci, i);
        assert(success);
        verifyFunction(*newProc);
    }    
    
    //newProc->dump();
    return newProc;
}


// =============================================================================
// MSG
// 
// Print the given message 
// if the user did not use -dis-opt-msg
// =============================================================================
void TLMBasicPassImpl::MSG(std::string msg) {
    if(!this->disableMsg)
        std::cout << msg;
}

