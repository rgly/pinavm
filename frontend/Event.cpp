#include <string>

#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "Event.hpp"
#include "Process.hpp"

using namespace llvm;
using namespace std;

Event::Event(Process * p, string event)
{
	this->process = p;
	this->eventName = event;
}

string Event::toString()
{
	return this->eventName;
}

Process *Event::getProcess()
{
	return this->process;
}

/********** Pretty print **********/
void Event::printElab(int sep, string prefix)
{
	this->printPrefix(sep, prefix);
	TRACE("Event : " << this << " (\"" << this->toString() << "\")\n");
}
