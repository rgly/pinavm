#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/Streams.h"

#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "NotifyHandler.hpp"

using namespace llvm;

SCConstruct*
NotifyHandler::handle(Function* fct, BasicBlock* bb, CallInst* callInst)
{
  char* eventName = "eventName";
  llvm::cout << " ---------------> Time : handling call to notify(event) performed in " << fct->getName() <<"\n";
  return new NotifyConstruct(new Event(eventName));
}

void
NotifyHandler::insertInMap(std::map<Function*, SCConstructHandler*>* scchandlers)
{
  SCConstructHandler::insertInMap(scchandlers, "_ZN7sc_core8sc_event6notifyEv");
}
