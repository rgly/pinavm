#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/Streams.h"

#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "EventHandler.hpp"

using namespace llvm;

SCConstruct*
EventHandler::handle(Function* fct, BasicBlock* bb, CallInst* callInst)
{
  llvm::cout << " ---------------> Event : handling call to wait(event) performed in " << fct->getName() <<"\n";
  return new EventConstruct(new Event("FAKE EVENT"));
}

void
EventHandler::insertInMap(std::map<Function*, SCConstructHandler*>* scchandlers)
{
  SCConstructHandler::insertInMap(scchandlers, "_ZN7sc_core9sc_module4waitERKNS_8sc_eventE");
}
