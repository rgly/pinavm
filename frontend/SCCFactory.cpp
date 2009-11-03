#include "SCCFactory.hpp"

SCCFactory::SCCFactory(SCJit* scjit)
{
  /****** Time stuffff *******/
  (new ZeroTimeHandler(scjit))->insertInMap(&this->scchandlers);
  (new DefaultTimeHandler(scjit))->insertInMap(&this->scchandlers);
  
  /****** Events stuff ******/
  (new EventHandler(scjit))->insertInMap(&this->scchandlers);
  (new NotifyHandler(scjit))->insertInMap(&this->scchandlers);
  
  /***** Ports stuff ******/
  (new WriteHandler(scjit))->insertInMap(&this->scchandlers);
  (new ReadHandler(scjit))->insertInMap(&this->scchandlers);
}

bool
SCCFactory::handle(llvm::Function* fct, BasicBlock* bb, CallInst* callInst)
{
  Function* calledFct;
  SCConstructHandler* scch;
  
  calledFct = callInst->getCalledFunction();

  std::map<Function*,SCConstructHandler*>::iterator it = this->scchandlers.find(calledFct);
  if (it != scchandlers.end()) {
    scch = it->second;
    std::cout << "handling!\n";
    this->scc[callInst] = scch->handle(fct, bb, callInst);
    return true;
  } else {
    return false;
  }
}
