#include "SCJit.hpp"
#include "NotifyHandler.hpp"

using namespace llvm;

SCConstruct *NotifyHandler::handle(Function * fct, BasicBlock * bb, Instruction* callInst, Function* calledFunction)
{
	string eventName = "eventName";
	TRACE_3("Handling call to notify(event)\n");

	Value *arg = callInst->getOperand(1);
	void *eventAddr = this->scjit->jitAddr(fct, callInst, arg);
	TRACE_4("Address jitted : " << eventAddr << "\n");
	Event *e = this->scjit->getElab()->getEvent(eventAddr);
	TRACE_3("Event notified : " << (void *) e << " (" << e->toString() << ") \n");
	return new NotifyConstruct(e);

}

void NotifyHandler::insertInMap(std::map < Function *,
				SCConstructHandler * >*scchandlers)
{
	SCConstructHandler::insertInMap(scchandlers,
					"_ZN7sc_core8sc_event6notifyEv");
}
