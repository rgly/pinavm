#include "NotifyConstruct.hpp"
#include "Event.hpp"

using namespace llvm;

NotifyConstruct::NotifyConstruct(Event * e)
{
	this->event = e;
	this->id = NOTIFYCONSTRUCT;
}

NotifyConstruct::NotifyConstruct(Value * missingE) : SCConstruct(false)
{
	this->event = NULL;
	this->missingEvent = missingE;
	this->id = NOTIFYCONSTRUCT;
}

void
 NotifyConstruct::setNotifiedEvent(Event * e)
{
	this->event = e;
}

Event *NotifyConstruct::getNotifiedEvent()
{
	return this->event;
}

std::string NotifyConstruct::toString()
{
	if (this->staticallyFound)
		return "NOTIFY(" + this->event->getEventName() + ")";
	else
		return "NOTIFY(dynamic content)";
}
