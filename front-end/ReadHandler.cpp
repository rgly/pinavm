#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "ReadHandler.hpp"

using namespace llvm;

SCConstruct*
ReadHandler::handle(Function* fct, BasicBlock* bb, CallInst* callInst)
{
  Port* p = new Port("FAKE MODULE", "FAKE PORT");
  llvm::cout << " ---------------> Time : handling call to read() performed in " << fct->getName() <<"\n";
  return new ReadConstruct(p);
}


void
ReadHandler::insertInMap(std::map<Function*, SCConstructHandler*>* scchandlers)
{
  SCConstructHandler::insertInMap(scchandlers, "_ZNK7sc_core5sc_inIbE4readEv");
}
