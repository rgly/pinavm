#include "NotifyHandler.hpp"

using namespace llvm;

SCConstruct *NotifyHandler::handle(Function * fct, BasicBlock * bb,
				   CallInst * callInst)
{
<<<<<<< HEAD
  string eventName = "eventName";
  TRACE_3("Handling call to notify(event)\n");

  Value* arg = callInst->getOperand(1);
  void* eventAddr = this->scjit->jitAddr(fct, arg);
  TRACE_4("Address jitted : " << eventAddr << "\n"); 
  Event* e = this->scjit->getElab()->getEvent(eventAddr);
  TRACE_3("Event notified : " << (void*) e << "\n"); 
  return new NotifyConstruct(e);

=======
	string eventName = "eventName";
	std::
	    cout <<
	    " ---------------> Time : handling call to notify(event) performed in "
	    << fct->getNameStr() << "\n";
	return new NotifyConstruct(new SCEvent(eventName));
>>>>>>> add3574c50f6a83278ac770d8f023da518821eba
}

void NotifyHandler::insertInMap(std::map < Function *,
				SCConstructHandler * >*scchandlers)
{
	SCConstructHandler::insertInMap(scchandlers,
					"_ZN7sc_core8sc_event6notifyEv");
}
