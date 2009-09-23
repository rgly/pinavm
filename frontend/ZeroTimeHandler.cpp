#include "ZeroTimeHandler.hpp"

using namespace llvm;

SCConstruct*
ZeroTimeHandler::handle(Function* fct, BasicBlock* bb, CallInst* callInst)
{
  cout << " ---------------> ZeroTime : handling call to wait() performed in " << fct->getNameStr() <<"\n";
  return new ZeroTimeConstruct();
}

void
ZeroTimeHandler::insertInMap(std::map<Function*, SCConstructHandler*>* scchandlers)
{
  SCConstructHandler::insertInMap(scchandlers, "_ZN7sc_core9sc_module4waitEv");
}
