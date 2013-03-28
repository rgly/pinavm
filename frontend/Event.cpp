#include "Event.hpp"
#include "config.h"

struct Process;

static int numberOfEvents = 1;

Event::Event(std::string name)
{
  this->eventName = name;
  this->numEvent = numberOfEvents++;
}

std::string Event::getEventName()
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
void Event::printElab(int sep, std::string prefix)
{
	this->printPrefix(sep, prefix);
	TRACE("Event : " << this << " (\"" << this->getEventName() << "\")\n");
}
