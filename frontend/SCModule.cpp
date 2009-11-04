#include "SCModule.hpp"

SCModule::SCModule(string moduleName)
{
	this->name = moduleName;
}

/******** Threads ********/
vector < Function * >*SCModule::getThreads()
{
	return &this->threads;
}

void SCModule::addThread(Function * mainFct)
{
	this->threads.push_back(mainFct);
}


/******** Ports ********/
vector < SCPort * >*SCModule::getPorts()
{
	return &this->ports;
}

void SCModule::addPort(SCPort * port)
{
	this->ports.push_back(port);
}


/******** Shared Variables ********/
vector < GlobalVariable * >*SCModule::getSharedVariables()
{
	return &this->sharedVariables;
}

void SCModule::addSharedVariable(GlobalVariable * gv)
{
	this->sharedVariables.push_back(gv);
}
