#include "ZeroTimeHandler.hpp"
#include "ZeroTimeConstruct.hpp"

using namespace llvm;

SCConstruct *ZeroTimeHandler::handle(Function * fct, BasicBlock * bb, Instruction* callInst, Function* calledFunction)
{
	TRACE_3("Handling call to wait()\n");
	return new ZeroTimeConstruct();
}

void ZeroTimeHandler::insertInMap(std::map < Function *, SCConstructHandler * >*scchandlers)
{
	// sc_core::sc_module::wait()
	// MM: TODO: wait() is _not_ a zero-time wait. wait() without
	// argument means "wait for something in the static
	// sensitivity list"
	SCConstructHandler::insertInMap(scchandlers,
					"_ZN7sc_core9sc_module4waitEv");
}
