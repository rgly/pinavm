#include "EventHandler.hpp"

using namespace llvm;

SCConstruct *EventHandler::handle(Function * fct, BasicBlock * bb,
				  CallInst * callInst)
{
	std::
	    cout <<
	    " ---------------> Event : handling call to wait(event) performed in "
	    << fct->getNameStr() << "\n";
	return new EventConstruct(new SCEvent("FAKE EVENT"));
}

void EventHandler::insertInMap(std::map < Function *,
			       SCConstructHandler * >*scchandlers)
{
	SCConstructHandler::insertInMap(scchandlers,
					"_ZN7sc_core9sc_module4waitERKNS_8sc_eventE");
}
