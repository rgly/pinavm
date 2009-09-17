#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/Streams.h"

#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "ZeroTimeHandler.hpp"

using namespace llvm;

SCConstruct*
ZeroTimeHandler::handle(Function* fct, BasicBlock* bb, CallInst* callInst)
{
  llvm::cout << " ---------------> ZeroTime : handling call to wait() performed in " << fct->getName() <<"\n";
  return new ZeroTimeConstruct();
}

void
ZeroTimeHandler::insertInMap(std::map<Function*, SCConstructHandler*>* scchandlers)
{
  SCConstructHandler::insertInMap(scchandlers, "_ZN7sc_core9sc_module4waitEv");
}
