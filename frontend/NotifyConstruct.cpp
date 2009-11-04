#include "NotifyConstruct.hpp"

using namespace llvm;

NotifyConstruct::NotifyConstruct(SCEvent * e)
{
	this->event = e;
}

void
 NotifyConstruct::setNotifiedEvent(SCEvent * e)
{
	this->event = e;
}

SCEvent *NotifyConstruct::getNotifiedEvent()
{
	return this->event;
}

string NotifyConstruct::toString()
{
	return "NOTIFY(event)";
}
