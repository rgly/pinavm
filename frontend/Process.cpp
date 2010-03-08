#include "Process.hpp"
#include "IRModule.hpp"
#include "SCConstruct.hpp"
#include "SCCFactory.hpp"
#include "utils.h"
#include "llvm/Intrinsics.h"

#include <algorithm>
#include <vector>

using namespace std;

static int nbProcesses = 0;

Process::Process(IRModule * mod, Function * mainFunc, string name, string funcName)
{
	this->module = mod;
	this->mainFct = mainFunc;
	this->fctName = funcName;
	this->pid = nbProcesses++;
	this->processName = name + "_pid" + intToString(this->pid);
}

int Process::getPid()
{
	return this->pid;
}

string Process::getFctName()
{
	return this->fctName;
}

string Process::getName()
{
	return this->processName;
}

Function *Process::getMainFct()
{
	return this->mainFct;
}

IRModule *Process::getModule()
{
	return this->module;
}


void Process::addEvent(Event * ev)
{
	this->events.push_back(ev);
}
std::vector < Event* >*
Process::getEvents()
{
	return & this->events;
}

/********** Pretty print **********/
void Process::printElab(int sep, string prefix)
{
	std::vector < Event * >::iterator itE;
	this->printPrefix(sep, prefix);
	TRACE(" process : " << this << "\n");
	for (itE = this->events.begin(); itE < this->events.end(); itE++) {
		Event *e = *itE;
		e->printElab(sep + 3, prefix);
	}
}


void Process::printIR(SCCFactory * sccfactory)
{
	Function *F;
	bool printBB = false;
	std::vector < Function * >*fctStack = new std::vector < Function * >();
	fctStack->push_back(this->mainFct);
	std::map < CallInst *, std::map<Process*, SCConstruct * > >* constructs = sccfactory->getConstructs();
	std::map < CallInst *, std::map<Process*, SCConstruct * > >::iterator itC;

	while (!fctStack->empty()) {
		F = fctStack->back();
		fctStack->pop_back();
		TRACE("Function : " << F->getNameStr() << "\n");
		for (Function::iterator bb = F->begin(), be = F->end(); bb != be; ++bb) {
			TRACE("   BasicBlock : " << bb->getNameStr() << "\n");
			BasicBlock::iterator i = bb->begin(), ie = bb->end();
			printBB = false;
			while (i != ie) {
				CallInst *callInst = dyn_cast < CallInst > (&*i);
				if (printBB) {
					printBB = false;
					TRACE("   (stay in block " << bb->getNameStr() << ")\n");
				}
				if (callInst) {
					if ((itC = constructs->find(callInst)) != constructs->end()) {
						map<Process*, SCConstruct*> CbyP = itC->second;
						SCConstruct *scc = CbyP.find(this)->second;
						TRACE("    SCC Construct : " << scc->toString() << "\n");
					} else if (callInst->getCalledFunction()->getIntrinsicID() == Intrinsic::not_intrinsic) {
						fctStack->push_back(callInst->getCalledFunction());
					}
				}
				i++;
			}
		}
	}
}

std::vector < Function * >*Process::getUsedFunctions()
{
	return &this->usedFunctions;
}

void Process::addUsedFunction(Function * fct)
{
	if (find(this->usedFunctions.begin(), this->usedFunctions.end(), fct) == this->usedFunctions.end()) {
		this->usedFunctions.push_back(fct);
	}
}
