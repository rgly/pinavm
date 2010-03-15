#include "ZeroTimeHandler.hpp"

using namespace llvm;

SCConstruct *ZeroTimeHandler::handle(Function * fct, BasicBlock * bb, Instruction* callInst, Function* calledFunction)
{
	TRACE_3("Handling call to wait()\n");
	return new ZeroTimeConstruct();
}

void ZeroTimeHandler::insertInMap(std::map < Function *, SCConstructHandler * >*scchandlers)
{
	SCConstructHandler::insertInMap(scchandlers,
					"_ZN7sc_core9sc_module4waitEv");
}
