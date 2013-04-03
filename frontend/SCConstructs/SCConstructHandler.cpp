#include "SCConstructHandler.hpp"
#include "SCConstruct.hpp"
#include "SCJit.hpp"

using namespace llvm;

SCConstructHandler::SCConstructHandler()
{
}

SCConstructHandler::SCConstructHandler(SCJit * jit)
{
	this->scjit = jit;
}

void SCConstructHandler::insertInMap(std::map < Function *,
				     SCConstructHandler * >*scchandlers,
				     std::string fctName)
{
	Function *targetFct =
	    this->scjit->getModule()->getFunction(fctName);
	if (!targetFct) {
		TRACE_3("Handler for function " << fctName <<
			" : NOT found\n\n" );
	} else {
		TRACE_3("Handler for function " << fctName << " : "
			<< targetFct << '\n');
		scchandlers->insert(std::make_pair(targetFct, this));
	}
}
