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

                        IRModule *mod = pr->getModule();
                        Function *writef = lookForWriteFunction(mod);
                        Function *readf = lookForReadFunction(mod);
                        vector <Process *> *procs = mod->getProcesses();
                        if(procs->size()>0) {
                            std::vector <Process *>::iterator pIt;
                            for (pIt = procs->begin(); pIt<procs->end(); pIt++) {
                                Process *proc = *pIt;
                                std::cout << " proc : " << proc->getName()
                                << endl;
                                
                                
                                replaceCallsInProcess(proc, writef, readf);
                                
                            } 
                        }
                    }
            }
        }
        
	}
    std::cout << "\n===========================================\n\n";
}


// =============================================================================
// replaceCallsInProcess
// 
// Replace indirect calls to write() or read() by direct calls 
// in the given process.
// =============================================================================
int TLMBasicPass::replaceCallsInProcess(Process *proc, 
                                        Function *writef, Function *readf) {
   
    Function *procf = proc->getMainFct();
    inst_iterator ii;
    for (ii = inst_begin(procf); ii!=inst_end(procf); ii++) {
        Instruction &i = *ii;
        CallSite cs = CallSite::get(&i);
        if (cs.getInstruction()) {
            Function *oldfun = cs.getCalledFunction();
            if (oldfun!=NULL && !oldfun->isDeclaration()) {
                std::string name = oldfun->getName();
                std::string basename = name.substr(name.size()-13,name.size()-1);
                std::cout << "  Called function : " << name
                << " - " << basename << std::endl;
                //
                // Write
                //
                //if (!strcmp(basename.c_str(),std::string("5writeERKjS1_").c_str())) {
                std::string calledf("_ZN5basic21initiator_socket_baseILb0EE5writeERKjji");
                if (!strcmp(name.c_str(), calledf.c_str())) {
                
                    // Candidate for a replacement
                    
                    if(ConstantInt *ci = dyn_cast<ConstantInt>(cs.getArgument(2))) {
                        ci->getZExtValue();
                        //std::cout << "Addr = " << addr << endl;
                        std::cout << "  Addr OKI" << endl;
                    }
                    
                } else 
                //
                // Read
                //
                if (!strcmp(basename.c_str(),std::string("4readERKjRj").c_str())) {
                        
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

