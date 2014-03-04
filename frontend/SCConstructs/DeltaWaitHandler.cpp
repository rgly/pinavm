#include "SCJit.hpp"
#include "DeltaWaitConstruct.hpp"
#include "DeltaWaitHandler.hpp"
#include <llvm/Support/CallSite.h>

using namespace llvm;

SCConstruct *DeltaWaitHandler::handle(Function * fct, BasicBlock * bb, Instruction* callInst, Function* calledFunction)
{
	TRACE_3("Handling call to wait(int) or wait()");
	bool errb = false;

	Function* callee = CallSite(callInst).getCalledFunction();
	unsigned arg_num = callee->getArgumentList().size();
	// if wait()
	if (arg_num == 1)
		return new DeltaWaitConstruct(1);

	// arg0 is the module, argument 1 is the time to wait.
	Value *arg = CallSite(callInst).getArgument(1);

	if (arg->getType()->isIntegerTy()) {
		int time_waited = this->scjit->jitType<int>(fct, callInst, arg, &errb);
		// MM: TODO: check errb.
		TRACE_3("Int delta waited: " << time_waited << "\n");
		return new DeltaWaitConstruct(time_waited);
	} else
		ERROR("call to wait with argument not int\n");
}

void DeltaWaitHandler::insertInMap(std::map < Function *, SCConstructHandler * >*scchandlers)
{
	// sc_core::sc_module::wait(int)
	// i.e. wait for <int> delta-cycles.
	SCConstructHandler::insertInMap(scchandlers,
					"_ZN7sc_core9sc_module4waitEi");
	// sc_core::sc_module::wait()
	// i.e. wait for 1 delta-cycles.
	SCConstructHandler::insertInMap(scchandlers,
					"_ZN7sc_core9sc_module4waitEv");
}
