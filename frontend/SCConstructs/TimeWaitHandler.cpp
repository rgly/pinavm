#include "SCJit.hpp"
#include "TimeWaitConstruct.hpp"
#include "TimeWaitHandler.hpp"
#include <llvm/IR/CallSite.h>
#include "Time.hpp"


using namespace llvm;

SCConstruct *TimeWaitHandler::handle(Function * fct, BasicBlock * bb, Instruction* callInst, Function* calledFunction)
{
	TRACE_3("Handling call to wait(time)");
	// arg0 is the module, argument 1 is the time to wait.
	Value *arg = CallSite(callInst).getArgument(1);
	bool errb = false;

	Value* arg2 = NULL;
	Function* callee = CallSite(callInst).getCalledFunction();
	unsigned arg_num = callee->getArgumentList().size();
	if (arg_num > 2)
		arg2 = CallSite(callInst).getArgument(2);


	if (arg->getType()->isFloatingPointTy()) {
		double time_waited = this->scjit->jitType<double>(fct, callInst, arg, &errb);
		// MM: TODO: check errb.
		TRACE_3("Double time waited: " << time_waited << "\n");

		if (arg2) {
			sc_core::sc_time_unit time_u =
				this->scjit->jitType<sc_core::sc_time_unit>(fct, callInst, arg2, &errb);

			return new TimeWaitConstruct(
				pinavm::Time(time_waited, time_u));
		} else {
			return new TimeWaitConstruct(
				pinavm::Time(time_waited, pinavm::Time::NS));
		}
	} else {
		ERROR("call to wait with argument not double\n");
	}
}

void TimeWaitHandler::insertInMap(std::map < Function *, SCConstructHandler * >*scchandlers)
{
	// sc_core::sc_module::wait(double, sc_core::sc_time_unit)
	// i.e. wait for some time.
	SCConstructHandler::insertInMap(scchandlers,
					"_ZN7sc_core9sc_module4waitEdNS_12sc_time_unitE");

	// sc_core::wait(double, sc_core::sc_time_unit, sc_core::sc_simcontext*)
	// wait(double, sc_time_unit) seems to be replaced by this
	// sometimes.
	// MM: TODO: this is sc_core::wait, hence not a member
	// function. Relevant argument is probably arg 0 instead of arg 1 here.
	SCConstructHandler::insertInMap(scchandlers,
					"_ZN7sc_core4waitEdNS_12sc_time_unitEPNS_13sc_simcontextE");
}
