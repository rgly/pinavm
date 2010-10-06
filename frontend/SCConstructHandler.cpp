#include "SCConstructHandler.hpp"
#include "SCConstruct.hpp"
#include "SCJit.hpp"

using namespace llvm;
using namespace std;

SCConstructHandler::SCConstructHandler()
{
}

SCConstructHandler::SCConstructHandler(SCJit * jit)
{
	this->scjit = jit;
}

void SCConstructHandler::insertInMap(std::map < Function *,
				     SCConstructHandler * >*scchandlers,
				     string fctName)
{
	Function *targetFct =
	    this->scjit->getModule()->getFunction(fctName);
	if (!targetFct) {
		TRACE_3("Handler for function " << fctName <<
			" : NOT found\n" << std::endl);
	} else {
		TRACE_3("Handler for function " << fctName << " : "
			<< targetFct << std::endl);
		scchandlers->insert(make_pair(targetFct, this));
	}
}
