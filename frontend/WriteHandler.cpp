#include "WriteHandler.hpp"

using namespace llvm;

SCConstruct*
WriteHandler::handle(Function* fct, BasicBlock* bb, CallInst* callInst)
{
  Port* p = new Port("FAKE MODULE", "FAKE PORT");
  cout << " ---------------> Time : handling call to write() performed in " << fct->getNameStr() <<"\n";
  return new WriteConstruct(p);
}


void
WriteHandler::insertInMap(std::map<Function*, SCConstructHandler*>* scchandlers)
{
  SCConstructHandler::insertInMap(scchandlers, "_ZN7sc_core8sc_inoutIbE5writeERKb");
}
