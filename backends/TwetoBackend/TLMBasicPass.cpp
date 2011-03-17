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


TLMBasicPass::TLMBasicPass(Frontend *fe) : ModulePass(ID) {
    this->fe = fe; 
    this->elab = fe->getElab();
}


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
                                // Look for 
                                // _ZN5basic21initiator_socket_baseILb0EE5writeERKjji
                                
                                
                            } 
                        }
                    }
            }
        }
        
	}
    std::cout << "\n===========================================\n\n";
    

    /*SCConstruct * scc = ...
     if(scc->getID()==WRITECONSTRUCT) {
     wc = (WriteConstruct*) scc;		
     port = wc->getPort();
     channels = port->getChannels();
     TRACE_3("Visit WRITE\n");
     if(port->getChannelID()==SIMPLE_CHANNEL) {
     
     }
     */
    
    // Replace all calls to write function
    /* std::vector <Function *> *funs = p->getUsedFunctions();
     std::vector <Function *>::iterator funIt;
     for (funIt = funs->begin(); funIt<funs->end(); funIt++) {
     Function *f = *funIt;
     std::cout << "== function : " << f->getNameStr() << std::endl;
     }*/
    
    
}
