#include "DefaultTimeHandler.hpp"
#include <ostream>

using namespace llvm;

SCConstruct *DefaultTimeHandler::handle(Function * fct, BasicBlock * bb,
					CallInst * callInst)
{
  TRACE_3("Handling call to wait(int)... ");
  Value* arg = callInst->getOperand(2);
  
  if (arg->getType()->isInteger()) {
    int time_waited = this->scjit->jitInt(fct, arg);
    TRACE_3("time waited: " << time_waited << "\n");
    return new DefaultTimeConstruct(time_waited);
  } else if (arg->getType()->isFloatingPoint()) {
    double time_waited = this->scjit->jitDouble(fct, arg);
    TRACE_3("time waited: " << time_waited << "\n");
    return new DefaultTimeConstruct(time_waited);
  } else {
    ERROR("call to wait with argument not int nor double\n");
  }
}

void DefaultTimeHandler::insertInMap(std::map < Function *,
				     SCConstructHandler * >*scchandlers)
{
  SCConstructHandler::insertInMap(scchandlers, "_ZN7sc_core9sc_module4waitEi");
  SCConstructHandler::insertInMap(scchandlers, "_ZN7sc_core9sc_module4waitEdNS_12sc_time_unitE");

}
