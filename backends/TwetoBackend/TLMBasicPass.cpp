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

#include "TLMBasicPass.h"


char TLMBasicPass::ID = 0;


// =============================================================================
// TLMBasicPass 
// 
// Constructor
// 
// =============================================================================
TLMBasicPass::TLMBasicPass(Frontend *fe) : ModulePass(ID) {
    this->fe = fe; 
    this->elab = fe->getElab();
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
        Port *pr = *portIt;
        std::cout << "== Port : " << pr->getName() << std::endl;
        // Looking for channel
        Channel *chan = pr->getChannel();
        if(chan!=NULL) {
            std::cout << "==    " << chan->toString() << std::endl;
            // Looking for the target port
            vector <Port *> *targets = this->elab->getPorts();
            std::vector <Port *>::iterator tarIt;
            for (tarIt = targets->begin(); tarIt<targets->end(); tarIt++) {
                Port *target = *tarIt;
                if(pr!=target)
                    if(pr->getChannelID()==target->getChannelID()) {
                        std::cout << "Bounded : " 
                        << pr->getName() << " -> " << target->getName()
                        << endl;
                        // Replace calls in all process
                        IRModule *mod = pr->getModule();
                        vector <Process *> *procs = mod->getProcesses();
                        if(procs->size()>0) {
                            std::vector <Process *>::iterator pIt;
                            for (pIt = procs->begin(); pIt<procs->end(); pIt++) {
                                Process *proc = *pIt;
                                std::cout << " proc : " << proc->getName()
                                << endl;
                            } 
                        }
                    }
            }
        }
        
	}
    std::cout << "\n===========================================\n\n";
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
    std::cout << "Looking for 'write' function in the module "
    << moduleName << std::endl;
    std::string name("_ZN"+moduleName.size()+moduleName+"5writeERKjS1_");
    Function *f = mod->getFunction(name);
    if(f!=NULL) {
        std::cout << "Found 'write' function in the module " 
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
    std::cout << "Looking for 'read' function in the module "
    << moduleName << std::endl;

    std::string name("_ZN"+moduleName.size()+moduleName+"4readERKjRj");    
    Function *f = mod->getFunction(name);
    if(f!=NULL) {
        std::cout << "Found 'read' function in the module " 
        << moduleName << std::endl;
    }
    return f;
}

