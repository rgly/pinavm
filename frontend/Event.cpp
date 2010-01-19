#include <string>

#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "Event.hpp"
#include "Process.hpp"

using namespace llvm;
using namespace std;

static int numberOfEvents = 0;

Event::Event(Process * p, string event)
{
	this->process = p;
	this->eventName = event;
	this->numEvent = numberOfEvents++;
}

string Event::toString()
{
	return this->eventName;
}

int
Event::getNumEvent()
{
	return this->numEvent;
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
