#include "SCJit.hpp"
#include "NotifyHandler.hpp"
#include "NotifyConstruct.hpp"
#include "Event.hpp"
#include <llvm/Support/CallSite.h>

using namespace llvm;


SCConstruct *NotifyHandler::handle(Function * fct, BasicBlock * bb, Instruction* callInst, Function* calledFunction)
{
	std::string eventName = "eventName";
	TRACE_3("Handling call to notify(event)\n");

	Value *arg = CallSite(callInst).getArgument(0);
	TRACE_4("We want to JIT this : ");
	arg->dump();
	TRACE_4("\n");
	TRACE_4("\n");

	void *eventAddr = this->scjit->jitAddr(fct, callInst, arg);
	TRACE_4("Address jitted : " << eventAddr << "\n");
	if (eventAddr == NULL)
		return new NotifyConstruct((Value*)NULL);
	Event *e = this->scjit->getElab()->getEvent(eventAddr);
	TRACE_3("Event notified : " << (void *) e << " (" << e->getEventName() << ") \n");
	return new NotifyConstruct(e);

}

void NotifyHandler::insertInMap(std::map < Function *,
				SCConstructHandler * >*scchandlers)
{
	SCConstructHandler::insertInMap(scchandlers,
					"_ZN7sc_core8sc_event6notifyEv");
}
