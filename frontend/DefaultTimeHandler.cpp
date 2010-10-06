#include "SCJit.hpp"
#include "DefaultTimeHandler.hpp"
#include "DefaultTimeConstruct.hpp"

#include <ostream>

using namespace llvm;

SCConstruct *DefaultTimeHandler::handle(Function * fct, BasicBlock * bb, Instruction* callInst, Function* calledFunction)
{
	TRACE_3("Handling call to wait(...)");
	Value *arg = callInst->getOperand(2);
	bool errb = false;

	if (arg->getType()->isIntegerTy()) {
		int time_waited = this->scjit->jitInt(fct, callInst, arg, &errb);
		// MM: TODO: check errb.
		// MM: TODO: this is _not_ a wait on time semantically.
		TRACE_3("Int time waited: " << time_waited << "\n");
		return new DefaultTimeConstruct(time_waited);
	} else if (arg->getType()->isFloatingPointTy()) {
		double time_waited = this->scjit->jitDouble(fct, callInst, arg, &errb);
		// MM: TODO: check errb.
		TRACE_3("Double time waited: " << time_waited << "\n");
		return new DefaultTimeConstruct(time_waited);
	} else {
		ERROR("call to wait with argument not int nor double\n");
	}
}

void DefaultTimeHandler::insertInMap(std::map < Function *, SCConstructHandler * >*scchandlers)
{
	// sc_core::sc_module::wait(int)
	// i.e. wait for <int> delta-cycles.
	SCConstructHandler::insertInMap(scchandlers,
					"_ZN7sc_core9sc_module4waitEi");
	// sc_core::sc_module::wait(double, sc_core::sc_time_unit)
	// i.e. wait for some time.
	SCConstructHandler::insertInMap(scchandlers,
					"_ZN7sc_core9sc_module4waitEdNS_12sc_time_unitE");
	// sc_core::wait(double, sc_core::sc_time_unit, sc_core::sc_simcontext*)
	// wait(double, sc_time_unit) seems to be replaced by this sometimes.
	SCConstructHandler::insertInMap(scchandlers,
					"_ZN7sc_core4waitEdNS_12sc_time_unitEPNS_13sc_simcontextE");
}
