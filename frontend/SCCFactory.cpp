#include "SCCFactory.hpp"
#include "Process.hpp"
#include "SCConstructs/SCConstruct.hpp"
#include "ALLHandler.h"

SCCFactory::SCCFactory(SCJit * scjit)
{
	/****** Time stuffff *******/
	(new ZeroTimeHandler(scjit))->insertInMap(&this->scchandlers);
	(new DefaultTimeHandler(scjit))->insertInMap(&this->scchandlers);

	/****** Events stuff ******/
	(new EventHandler(scjit))->insertInMap(&this->scchandlers);
	(new NotifyHandler(scjit))->insertInMap(&this->scchandlers);

	/***** Ports stuff ******/
	(new WriteHandler(scjit))->insertInMap(&this->scchandlers);
	(new ReadHandler(scjit))->insertInMap(&this->scchandlers);

	/***** Other stuff ******/
	(new AssertHandler(scjit))->insertInMap(&this->scchandlers);
//	(new RandHandler())->insertInMap(&this->scchandlers);

	/****** Basic protocol ******/
	(new BasicHandler(scjit))->insertInMap(&this->scchandlers);
}

SCCFactory::~SCCFactory()
{
	// for (map<Function*, SCConstructHandler*>::iterator it = this->scchandlers.begin(); it != this->scchandlers.end(); it++) {
	while (!this->scchandlers.empty()) {
		SCConstructHandler *scch =
		    this->scchandlers.begin()->second;
		this->scchandlers.erase(this->scchandlers.begin());
		delete scch;
	}

	for (std::map < Instruction *, std::map<Process*, SCConstruct *> >::iterator it =
		     this->scc.begin(); it != this->scc.end(); it++) {
		std::map<Process*, SCConstruct *> aMap = it->second;
		for (std::map<Process*, SCConstruct *>::iterator itM = aMap.begin(); itM != aMap.end(); ++itM) {
			SCConstruct *construct = itM->second;
			delete construct;
		}
		this->scc.erase(it);
	}
}

std::map <Instruction *, std::map<Process*, SCConstruct *> >* SCCFactory::getConstructs()
{
	return &this->scc;
}

bool
SCCFactory::handle(Process* proc, llvm::Function * fct, BasicBlock * bb, Instruction* callInst, Function* calledFunction)
{
	Function *calledFct;
	SCConstructHandler *scch;

	std::map < Function *, SCConstructHandler * >::iterator it;
	it = this->scchandlers.find(calledFunction);

	if (it != scchandlers.end()) {
		scch = it->second;
		this->scc[callInst][proc] = scch->handle(fct, bb, callInst, calledFunction);
		return true;
	} else {
		return false;
	}
}

bool
SCCFactory::handlerExists(llvm::Function * fct, BasicBlock * bb, Instruction* callInst, Function* calledFunction)
{
	return this->scchandlers.find(calledFunction) != this->scchandlers.end();
}
