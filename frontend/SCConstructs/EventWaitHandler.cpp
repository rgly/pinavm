#include <string>

#include "EventWaitHandler.hpp"
#include "EventWaitConstruct.hpp"
#include "SCJit.hpp"
#include <llvm/Support/CallSite.h>


using namespace llvm;

SCConstruct *EventWaitHandler::handle(Function * fct, BasicBlock * bb, Instruction* callInst, Function* calledFunction)
{
	TRACE_3("Handling call to wait(event)\n");

	// TODO: untested.
	Value *arg = CallSite(callInst).getArgument(1);
	TRACE_4("We want to JIT this : ");
	arg->dump();
	TRACE_4("\n");
	TRACE_4("\n");

	void *eventAddr = this->scjit->jitType<void*>(fct, callInst, arg, NULL);
	TRACE_4("Address jitted : " << eventAddr << "\n");
	if (eventAddr == NULL) {
		return new EventWaitConstruct(arg);
	} else {
		Event *e = this->scjit->getElab()->getEvent(eventAddr);
		TRACE_3("Event associated : " << (void *) e << " (" << e->getEventName() << ") \n");
		return new EventWaitConstruct(e);
	}
}

void EventWaitHandler::insertInMap(std::map < Function *, SCConstructHandler * >* scchandlers)
{
	SCConstructHandler::insertInMap(scchandlers, (std::string) "_ZN7sc_core9sc_module4waitERKNS_8sc_eventE");
}
