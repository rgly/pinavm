#include "NotifyHandler.hpp"

using namespace llvm;

SCConstruct*
NotifyHandler::handle(Function* fct, BasicBlock* bb, CallInst* callInst)
{
  string eventName = "eventName";
  cout << " ---------------> Time : handling call to notify(event) performed in " << fct->getNameStr() <<"\n";
  return new NotifyConstruct(new Event(eventName));
}

void
NotifyHandler::insertInMap(std::map<Function*, SCConstructHandler*>* scchandlers)
{
  SCConstructHandler::insertInMap(scchandlers, "_ZN7sc_core8sc_event6notifyEv");
}
