#include <string>

#include "IRModule.hpp"
#include "Port.hpp"

#include "config.h"

using namespace std;

Port::Port(IRModule * module, string portName)
{
	this->irModule = module;
	this->name = portName;
}

IRModule *Port::getModule()
{
	return this->irModule;
}

string Port::getName()
{
	return this->name;
}

/********** Pretty print **********/
void Port::printElab(int sep, string prefix)
{
	this->printPrefix(sep, prefix);
	//  TRACE("Port : " << (void*) this << " (\"" << this->name() << "\")\n");
}
