#include "NotifyConstruct.hpp"

using namespace llvm;

NotifyConstruct::NotifyConstruct(Event * e)
{
	this->event = e;
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

string NotifyConstruct::toString()
{
	if (this->staticallyFound)
		return "NOTIFY(" + this->event->toString() + ")";
	else
		return "NOTIFY(dynamic content)";
}
