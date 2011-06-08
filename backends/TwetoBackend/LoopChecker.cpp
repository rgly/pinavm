/**
 * LoopChecker.cpp
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : si-mohamed.lamraoui@imag.fr
 * @date : 2011/05/31
 * @copyright : Verimag 2011
 */

#include <iostream>
#include <string>

#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Pass.h"
#include "llvm/PassManagers.h"
#include "llvm/Function.h"

#include "LoopChecker.h"


char LoopChecker::ID = 0;


// =============================================================================
// LoopChecker 
// 
// 
// =============================================================================
LoopChecker::LoopChecker(std::vector<CallInfo*> *infos, bool &status) 
            : FunctionPass(ID) {
    this->infos = infos;
    this->status = status;
    this->status = true; // if there is no loop
}


// =============================================================================
// runOnFunction 
// 
// 
// =============================================================================
bool LoopChecker::runOnFunction(Function &F) {
    LoopInfo *li = &getAnalysis<LoopInfo>();
    for (LoopInfo::iterator I = li->begin(), E = li->end(); I != E; ++I) {
        Loop *l = *I; 
        bool status = isInvariant(l);
        if(status) {
            std::cout << "    > INVAR\n";   
        } else {
            std::cout << "    > VAR\n"; 
        }
    }
    return false;
}


// =============================================================================
// isInvariant 
// 
// If the address (addr) is modified inside the loop, return false, else true
// %x = add %y, 1
// store %x, %addr
// =============================================================================
bool LoopChecker::isInvariant(Loop *l) {
    /*std::vector<Instruction*> operators;
    std::vector<StoreInst*> stores;
    //std::cout << "= Loop ======================\n";
    Loop::block_iterator bb, e;
    for (bb = l->block_begin(), e = l->block_end(); bb != e; ++bb) {                    
        BasicBlock *current = *bb;
        //current->dump();
        BasicBlock::iterator ii, ie;
        for (ii = current->begin(), ie = current->end(); ii != ie; ++ii) {            
            Instruction &i = *ii;
            // Save operators
            if(i.isBinaryOp()) {
                operators.push_back(ii);
                //errs() << "Op : " << *ii << "\n";
            }
            // Save store instruction that uses addr
            StoreInst *si = dyn_cast<StoreInst>(ii);
            if(si!=NULL) {
                if(si->getPointerOperand()==this->addr) {
                    //errs() << "St : " << *ii << "\n";
                    stores.push_back(si);
                }
            }
        }
    }
    // Seeking a match
    std::vector<Instruction*>::iterator o;
    std::vector<StoreInst*>::iterator s;
    for (o = operators.begin(); o!=operators.end(); ++o) {
        for (s = stores.begin(); s!=stores.end(); ++s) {
            Value *r = dyn_cast<Value>(*o); // return value
            Value *v = (*s)->getValueOperand();
            if(r==v) {
                return false;
            }
        }
    }*/
    return true;
}

