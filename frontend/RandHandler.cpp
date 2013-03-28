#include "SCJit.hpp"
#include "RandHandler.h"
#include "RandConstruct.hpp"

using namespace llvm;


SCConstruct *RandHandler::handle(Function * fct, BasicBlock * bb, Instruction* callInst, Function* calledFunction)
{
	return new RandConstruct();
}

void RandHandler::insertInMap(std::map < Function *,
				SCConstructHandler * >*scchandlers)
{
	SCConstructHandler::insertInMap(scchandlers,
					"rand");
}
