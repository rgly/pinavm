#include "NotifyConstruct.hpp"

using namespace llvm;

NotifyConstruct::NotifyConstruct(Event* e)
{
	this->event = e;
}

void
NotifyConstruct::setNotifiedEvent(Event* e)
{
	this->event = e;
}

Event*
NotifyConstruct::getNotifiedEvent()
{
	return this->event;
}

string NotifyConstruct::toString()
{
	return "NOTIFY(event)";
}
