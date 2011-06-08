/**
 * LoopChecker.h
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : si-mohamed.lamraoui@imag.fr
 * @date : 2011/05/31
 * @copyright : Verimag 2011
 */

#ifndef LOOPCHECKER_H
#define LOOPCHECKER_H

#include <vector>

#include "llvm/Pass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopPass.h"

#include "TLMBasicPass.h"

using namespace llvm;


//============================================================================
class LoopChecker : public FunctionPass {

private:
    bool status;
    std::vector<CallInfo*> *infos;
    
public:
    static char ID;
    LoopChecker(std::vector<CallInfo*> *infos, bool &status);
    virtual bool runOnFunction(Function &F);
    bool isInvariant(Loop *l);
    virtual void getAnalysisUsage(AnalysisUsage &Info) const {
            Info.addRequired<LoopInfo>();
    }
    
};
//============================================================================


#endif