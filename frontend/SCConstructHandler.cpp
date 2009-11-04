#include "SCConstructHandler.hpp"

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
		std::
		    cout << "Info: targetFct " << fctName << " not found\n"
		    << std::endl;
	} else {
		std::
		    cout << "Function found: " << fctName << " Pointer: "
		    << targetFct << std::endl;
		scchandlers->insert(make_pair(targetFct, this));
	}
}
