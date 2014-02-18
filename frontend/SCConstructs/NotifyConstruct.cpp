#include "NotifyConstruct.hpp"
#include "Event.hpp"
#include <cassert>

using namespace llvm;

NotifyConstruct::NotifyConstruct(Event * e) : SCConstruct(NOTIFYCONSTRUCT)
{
	this->event = e;
}

NotifyConstruct::NotifyConstruct(Value * missingE)
	: SCConstruct(NOTIFYCONSTRUCT, false)
{
	this->event = NULL;
	this->missingEvent = missingE;
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
	if (this->isStaticallyFound()) {
		Event* e = this->getNotifiedEvent();
		assert(e);
		return "NOTIFY(" + e->getEventName() + ")";
	} else
		return "NOTIFY(dynamic content)";
}
