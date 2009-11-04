#include "DefaultTimeHandler.hpp"
#include <ostream>

using namespace llvm;

SCConstruct *DefaultTimeHandler::handle(Function * fct, BasicBlock * bb,
					CallInst * callInst)
{
	int time_waited = 42;
	std::
	    cout <<
	    " ---------------> DefaultTime : handling call to wait(int) performed in "
	    << fct->getNameStr() << "\n";
	Value *arg = callInst->getOperand(2);
	time_waited = this->scjit->jitInt(fct, arg);
	//  this->scjit->getModule(arg);
	//  arg = callInst->getOperand(2);
	std::cout << "   time waited: " << time_waited << "\n";
	return new DefaultTimeConstruct(time_waited);
}

void DefaultTimeHandler::insertInMap(std::map < Function *,
				     SCConstructHandler * >*scchandlers)
{
	SCConstructHandler::insertInMap(scchandlers,
					"_ZN7sc_core9sc_module4waitEi");
}
