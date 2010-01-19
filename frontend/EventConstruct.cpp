#include "EventConstruct.hpp"
#include "Event.hpp"

using namespace llvm;

EventConstruct::EventConstruct(Value* missingValue) : SCConstruct(missingValue)
{
	this->id = WAITEVENTCONSTRUCT;
}

EventConstruct::EventConstruct(Event * e)
{
	this->event = e;
	this->id = WAITEVENTCONSTRUCT;
}

void
 EventConstruct::setWaitedEvent(Event * e)
{
	this->event = e;
}

Event *EventConstruct::getWaitedEvent()
{
	return this->event;
}

std::string EventConstruct::toString()
{
	if (this->staticallyFound)
		return "WAIT(" + this->event->toString() + ")";
	else
		return "WAIT(dynamic content)";
}
