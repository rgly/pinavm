#include <string>
#include <vector>

#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "Event.hpp"
#include "Process.hpp"

using namespace llvm;
using namespace std;

static int numberOfEvents = 1;

Event::Event(string event)
{
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

std::vector<Process*>* Event::getProcesses()
{
	return &this->processes;
}

void
Event::addProcess(Process* p)
{
	this->processes.push_back(p);
}

/********** Pretty print **********/
void Event::printElab(int sep, string prefix)
{
	this->printPrefix(sep, prefix);
	TRACE("Event : " << this << " (\"" << this->toString() << "\")\n");
}
