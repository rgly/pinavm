#include "NotifyHandler.hpp"

using namespace llvm;

SCConstruct*
NotifyHandler::handle(Function* fct, BasicBlock* bb, CallInst* callInst)
{
  string eventName = "eventName";
  std::cout << " ---------------> Time : handling call to notify(event) performed in " << fct->getNameStr() <<"\n";
  return new NotifyConstruct(new SCEvent(eventName));
}

void
NotifyHandler::insertInMap(std::map<Function*, SCConstructHandler*>* scchandlers)
{
  SCConstructHandler::insertInMap(scchandlers, "_ZN7sc_core8sc_event6notifyEv");
}
