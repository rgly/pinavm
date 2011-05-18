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

#include <string>
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

#include "basic.h"
#include "bus.h"

#include "TLMBasicPass.h"


char TLMBasicPass::ID = 0;

// =============================================================================
// TLMBasicPass 
// 
// Constructor
// 
// =============================================================================
TLMBasicPass::TLMBasicPass(Frontend *fe) : ModulePass(ID) {
    this->optProcCounter = 0;
    this->fe = fe; 
    this->elab = fe->getElab();
    // Initialize function passes
    Module *mod = this->fe->getLLVMModule();
    TargetData *target = new TargetData(mod);
    funPassManager = new FunctionPassManager(mod);
    funPassManager->add(target);
    funPassManager->add(createIndVarSimplifyPass());
    funPassManager->add(createLoopUnrollPass());
    funPassManager->add(createInstructionCombiningPass());
    funPassManager->add(createReassociatePass());
    funPassManager->add(createGVNPass());
    funPassManager->add(createCFGSimplificationPass());
    funPassManager->add(createConstantPropagationPass());
}


// =============================================================================
// runOnModule
// 
//  
// 
// =============================================================================
bool TLMBasicPass::runOnModule(Module &M) {
    
    std::cout << "\n============== TLM Basic Pass =============\n";
    // Looking for ports
    vector <Port *> *ports = this->elab->getPorts();
    std::vector <Port *>::iterator portIt;
    for (portIt = ports->begin(); portIt<ports->end(); portIt++) {
        Port *port = *portIt;
        std::cout << "== Port : " << port->getName() << std::endl;
        // Looking for channel
        Channel *chan = port->getChannel();
        if(chan!=NULL) {
            std::cout << "==  " << chan->toString() << std::endl;
            // Looking for the target port
            vector <Port *> *targets = this->elab->getPorts();
            std::vector <Port *>::iterator tarIt;
            for (tarIt = targets->begin(); tarIt<targets->end(); tarIt++) {
                Port *target = *tarIt;
                optimize(chan, port, target);
            }
        }
	}
    std::cout << "===========================================\n\n";
}


// =============================================================================
// optimize
// 
// Optimize all process that use write or read functions
// =============================================================================
void TLMBasicPass::optimize(Channel *chan, Port *port, Port *target) {
    if(port!=target && port->getChannelID()==target->getChannelID()) {
        std::cout << "      " 
        << port->getName() << " -> " << target->getName()
        << std::endl;
        
        IRModule *mod = port->getModule();
        Function *writef = lookForWriteFunction(mod);
        Function *readf = lookForReadFunction(mod);
        vector <Process *> *procs = mod->getProcesses();
        if(procs->size()>0) {
            // Looking for calls in process
            std::vector <Process *>::iterator pIt;
            for (pIt = procs->begin(); pIt<procs->end(); pIt++) {
                Process *proc = *pIt;
                std::cout << "      proc : " << proc->getName()
                << std::endl;
                replaceCallsInProcess(proc, chan, writef, readf);
            } 
        }
    }
}


// =============================================================================
// replaceCallsInProcess
// 
// Replace indirect calls to write() or read() by direct calls 
// in the given process.
// =============================================================================
int TLMBasicPass::replaceCallsInProcess(Process *proc, Channel *chan,
                                        Function *writef, Function *readf) {
   
    // Get associate function
    Function *procf = proc->getMainFct();
    // Run preloaded passes on the function to propagate constants
    funPassManager->run(*procf);
    verifyFunction(*procf);
    
    inst_iterator ii;
    for (ii = inst_begin(procf); ii!=inst_end(procf); ii++) {
        Instruction &i = *ii;
        CallSite cs = CallSite::get(&i);
        if (cs.getInstruction()) {
            Function *oldfun = cs.getCalledFunction();
            if (writef!=NULL && oldfun!=NULL && !oldfun->isDeclaration()) {
                std::string name = oldfun->getName();
                std::string basename = name.substr(name.size()-13,name.size()-1);
                std::cout << "       Called function : " << name
                << " - " << basename << std::endl;
                //
                // Write
                //
                //if (!strcmp(basename.c_str(),std::string("5writeERKjS1_").c_str())) {
                std::string calledf("_ZN5basic21initiator_socket_baseILb0EE5writeERKjji");
                // Candidate for a replacement
                if (!strcmp(name.c_str(), calledf.c_str())) {
                    
                    std::cout << "       Checking adresses : ";
                    // Retreive the argument by executing 
                    // the appropriated piece of code
                    SCJit *scjit = new SCJit(this->fe->getLLVMModule(), this->elab);
                    scjit->setCurrentProcess(proc);
                    bool errb = false;
                    Value *arg = cs.getArgument(1);
                    Instruction *inst = cs.getInstruction();
                    int value = scjit->jitInt(procf, inst, arg, &errb);
                    std::cout << "    Addr = 0x0" << std::hex << value << std::endl;
                    
                    // Checking address range
                    Bus *bus = this->elab->getBus(chan);
                    basic::addr_t a = static_cast<basic::addr_t>(value);
                    bool addrErr = bus->checkAdressRange(a);
                    if(!addrErr) {
                        std::cerr << "  no target at address " <<
                        std::hex << value << std::endl;
                        return -1;
                    }
                    
                    // Replace the call
                    const unsigned n = oldfun->getFunctionType()->getNumParams();
                    Value **args_keep = new Value*[n];
                    Value **args_keep_end = args_keep;
                    for (unsigned i = 0; i!=n; ++i) {
                        *args_keep_end = cs.getArgument(i);
                        ++args_keep_end;
                    }
                    if (cs.isInvoke()) {
                        InvokeInst *i = dyn_cast<InvokeInst>(cs.getInstruction());
                        assert(i);
                        BasicBlock *bb1 = i->getNormalDest();
                        BasicBlock *bb2 = i->getUnwindDest();
                        InvokeInst *newinvoke =
                        InvokeInst::Create(writef,bb1,bb2,args_keep,args_keep_end,"",i);
                        BasicBlock::iterator ii_r(i);
                        ReplaceInstWithValue(i->getParent()->getInstList(),
                                             ii_r, newinvoke);
                    } else {
                        CallInst *newcall =
                        CallInst::Create(writef,args_keep,
                                         args_keep_end,"",cs.getInstruction());
                        BasicBlock::iterator ii_r(cs.getInstruction());
                        ReplaceInstWithValue(cs.getInstruction()->getParent()->getInstList(), 
                                             ii_r, newcall);
                    }
                    optProcCounter++;
                    std::cout << "       Call optimized (^_^)" << std::endl;
                    
                } else 
                //
                // Read
                //
                if (readf!=NULL && !strcmp(basename.c_str(),std::string("4readERKjRj").c_str())) {
                        
                    // Not yet supported
                                        
                }
            }  
        }
    }
    
    return 0;
}


// =============================================================================
// lookForWriteFunction
// 
// Look for an write function definition 
// in the given SystemC module
// =============================================================================
Function* TLMBasicPass::lookForWriteFunction(IRModule *module) {
    Module *mod = this->fe->getLLVMModule();
    // Reverse mangling
    std::string moduleName = module->getModuleType();
    std::cout << "      Looking for 'write' function in the module "
    << moduleName << std::endl;
    
    std::string name("_ZN"+moduleName+"5writeERKjS1_");
    Function *f = mod->getFunction(name);
    if(f!=NULL) {
        std::cout << "      Found 'write' function in the module " 
        << moduleName << std::endl;
    }
    return f;
}


// =============================================================================
// lookForWriteFunction
// 
// Look for an read function definition 
// in the given SystemC module
// =============================================================================
Function* TLMBasicPass::lookForReadFunction(IRModule *module) {
    Module *mod = this->fe->getLLVMModule();
    // Reverse mangling
    std::string moduleName = module->getModuleType();
    std::cout << "      Looking for 'read' function in the module "
    << moduleName << std::endl;

    std::string name("_ZN"+moduleName+"4readERKjRj");    
    Function *f = mod->getFunction(name);
    if(f!=NULL) {
        std::cout << "      Found 'read' function in the module " 
        << moduleName << std::endl;
    }
    return f;
}

