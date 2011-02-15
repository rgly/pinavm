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

	std::map < Instruction *, std::map<Process*, SCConstruct * > >* constructs = sccfactory->getConstructs();
	std::map < Instruction *, std::map<Process*, SCConstruct * > >::iterator itC;
	
	for (std::vector < Function * >::iterator itF = this->usedFunctions.begin(); itF < this->usedFunctions.end(); ++itF) {
		Function *F = *itF;
		TRACE("Function : " << F->getNameStr() << "\n");
		for (Function::iterator bb = F->begin(), be = F->end(); bb != be; ++bb) {
			TRACE("   BasicBlock : " << bb->getNameStr() << "\n");
			BasicBlock::iterator i = bb->begin(), ie = bb->end();
			printBB = false;
			bool isACall = false;
			while (i != ie) {
				Function* calledFunction;
				Instruction* currentInst = &*i;
				if (CallInst *callInst = dyn_cast < CallInst > (currentInst)) {
					calledFunction = callInst->getCalledFunction();
					isACall = true;
				} else if (InvokeInst *invokeInst = dyn_cast < InvokeInst > (currentInst)) {
					calledFunction = invokeInst->getCalledFunction();
					isACall = true;
				}
				if (printBB) {
					printBB = false;
				}
				if (isACall) {
					if ((itC = constructs->find(currentInst)) != constructs->end()) {
						map<Process*, SCConstruct*> CbyP = itC->second;
						SCConstruct *scc = CbyP.find(this)->second;
						//						TRACE("    ---> SCC Construct : " << scc->toString() << "\n");
					} else if (! calledFunction) {
						TRACE("    Call function pointer\n");
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
