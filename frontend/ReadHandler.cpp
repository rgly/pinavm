#include "SCJit.hpp"
#include "ReadHandler.hpp"
#include "Port.hpp"
#include "ReadConstruct.hpp"

using namespace llvm;

SCConstruct *ReadHandler::handle(Function * fct, BasicBlock * bb,
				 CallInst * callInst)
{
	TRACE_3("Handling call to read()\n");
	Value *arg = callInst->getOperand(2);
	void *portAddr = this->scjit->jitAddr(fct, arg);
	TRACE_4("Address jitted : " << portAddr << "\n");
	Port *p = this->scjit->getElab()->getPort(portAddr);
	TRACE_3("Port read : " << p << "\n");
	return new ReadConstruct(p);
}


void ReadHandler::insertInMap(std::map < Function *,
			      SCConstructHandler * >*scchandlers)
{
	SCConstructHandler::insertInMap(scchandlers,
					"_ZNK7sc_core5sc_inIbE4readEv");
}
