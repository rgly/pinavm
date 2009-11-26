#include <string>

#include "EventHandler.hpp"
#include "EventConstruct.hpp"
#include "SCJit.hpp"

using namespace llvm;

SCConstruct *EventHandler::handle(Function * fct, BasicBlock * bb,
				  CallInst * callInst)
{
  TRACE_3("Handling call to wait(event)\n");
  
  Value* arg = callInst->getOperand(2);
  void* eventAddr = this->scjit->jitAddr(fct, arg);
  TRACE_4("Address jitted : " << eventAddr << "\n"); 
  Event* e = this->scjit->getElab()->getEvent(eventAddr);
  TRACE_3("Event associated : " << (void*) e << "\n"); 
  return new EventConstruct(e);
}

void EventHandler::insertInMap(std::map < Function *,
			       SCConstructHandler * >*scchandlers)
{
	SCConstructHandler::insertInMap(scchandlers,
					(std::string) "_ZN7sc_core9sc_module4waitERKNS_8sc_eventE");
}
