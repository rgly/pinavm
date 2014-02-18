#include "EventWaitConstruct.hpp"

using namespace llvm;

EventWaitConstruct::EventWaitConstruct(Value* missingE)
    : WaitConstruct(EVENT, false), missingEvent(missingE)
{}

EventWaitConstruct::EventWaitConstruct(Event * e)
    : WaitConstruct(EVENT), event(e)
{}

Event *EventWaitConstruct::getEvent() const
{
	return this->event;
}

Value*
EventWaitConstruct::getMissingEvent()
{
	return this->missingEvent;
}

std::string EventWaitConstruct::toString()
{
	if (this->isStaticallyFound())
		return "WAIT(" + this->event->getEventName() + ")";
	else
		return "WAIT(dynamic content)";
}
