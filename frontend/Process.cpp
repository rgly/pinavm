#include "Process.hpp"
#include "IRModule.hpp"
#include "SCConstruct.hpp"
#include "SCCFactory.hpp"

#include <algorithm>
#include <vector>

using namespace std;

Process::Process(IRModule * mod, Function * mainFunc, string name,
		 string funcName)
{
	this->module = mod;
	this->mainFct = mainFunc;
	this->processName = name;
	this->fctName = funcName;
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


void Process::addPort(Port* p)
{
	this->ports.push_back(p);
}
std::vector < Port* >*
Process::getPorts()
{
	return & this->ports;
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
	std::vector < Function * >*fctStack =
	    new std::vector < Function * >();
	fctStack->push_back(this->mainFct);
	std::map < CallInst *, SCConstruct * >*constructs =
	    sccfactory->getConstructs();
	std::map < CallInst *, SCConstruct * >::iterator itC;

	while (!fctStack->empty()) {
		F = fctStack->back();
		fctStack->pop_back();
		TRACE("Function : " << F->getNameStr() << "\n");
		for (Function::iterator bb = F->begin(), be = F->end();
		     bb != be; ++bb) {
			TRACE("   BasicBlock : " << bb->
			      getNameStr() << "\n");
			BasicBlock::iterator i = bb->begin(), ie =
			    bb->end();
			printBB = false;
			while (i != ie) {
				CallInst *callInst =
				    dyn_cast < CallInst > (&*i);
				if (printBB) {
					printBB = false;
					TRACE("   (stay in block " << bb->
					      getNameStr() << ")\n");
				}
				if (callInst) {
					if ((itC =
					     constructs->find(callInst)) !=
					    constructs->end()) {
						SCConstruct *scc =
						    itC->second;
						TRACE
						    ("    SCC Construct : "
						     << scc->
						     toString() << "\n");
					} else {
						fctStack->
						    push_back(callInst->
							      getCalledFunction
							      ());
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
