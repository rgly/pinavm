#include "ReadHandler.hpp"

using namespace llvm;

SCConstruct*
ReadHandler::handle(Function* fct, BasicBlock* bb, CallInst* callInst)
{
  SCPort* p = new SCPort(NULL, "FAKE PORT");
  std::cout << " ---------------> Time : handling call to read() performed in " << fct->getNameStr() <<"\n";
  return new ReadConstruct(p);
}


void
ReadHandler::insertInMap(std::map<Function*, SCConstructHandler*>* scchandlers)
{
  SCConstructHandler::insertInMap(scchandlers, "_ZNK7sc_core5sc_inIbE4readEv");
}
