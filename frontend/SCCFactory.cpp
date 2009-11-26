#include "SCCFactory.hpp"

SCCFactory::SCCFactory(SCJit * scjit)
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

SCCFactory::~SCCFactory()
{
  // for (map<Function*, SCConstructHandler*>::iterator it = this->scchandlers.begin(); it != this->scchandlers.end(); it++) {
  while (! this->scchandlers.empty()) { 
    SCConstructHandler* scch = this->scchandlers.begin()->second;
    this->scchandlers.erase(this->scchandlers.begin());
    delete scch;
  }
    
  for (std::map<CallInst*, SCConstruct*>::iterator it = this->scc.begin(); it != this->scc.end(); it++) {
    SCConstruct* construct = it->second;
    this->scc.erase(it);
    delete construct;
  }
}

std::map<CallInst*, SCConstruct*>*
SCCFactory::getConstructs()
{
  return & this->scc;
}


bool
SCCFactory::handle(llvm::Function* fct, BasicBlock* bb, CallInst* callInst)
{
	Function *calledFct;
	SCConstructHandler *scch;

	calledFct = callInst->getCalledFunction();

  std::map<Function*,SCConstructHandler*>::iterator it = this->scchandlers.find(calledFct);
  if (it != scchandlers.end()) {
    scch = it->second;
    this->scc[callInst] = scch->handle(fct, bb, callInst);
    return true;
  } else {
    return false;
  }
}
