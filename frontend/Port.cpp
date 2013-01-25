#include <algorithm>
#include "Port.hpp"
#include "config.h"
#include "SCElab.h"


Port::Port(IRModule * module, std::string portName, sc_core::sc_port_base* sc_port_)
{
	this->irModule = module;
	this->name = portName;
	this->channel = NULL;
	this->channels = new std::vector<Channel*>();
	this->channelID = UNDEFINED_CHANNEL;
	this->sc_port = sc_port_ ;
}

IRModule *Port::getModule()
{
	return this->irModule;
}

std::string Port::getName()
{
	return this->name;
}

std::vector<Channel*>*
Port::getChannels()
{
	return this->channels;
}

Channel*
Port::getChannel()
{
	return this->channel;
}

void
Port::addChannel(Channel* ch)
{
	if (std::find(this->channels->begin(), this->channels->end(), ch) != this->channels->end()) {
		ERROR("Channel added twice to the same port : " << ch << "\n");
	} else if (this->channelID == UNDEFINED_CHANNEL) {
		this->channelID = ch->getID();
		this->type = ch->getType();
		this->channel = ch;
	} else if (this->channelID != ch->getID()) {
		ERROR("Cannot add different types of Channel to the same port\n");
	}
	this->channels->push_back(ch);
}

channel_id
Port::getChannelID()
{
	return this->channelID;
}

Type*
Port::getType()
{
	return this->type;
}

/********** Pretty print **********/
void Port::printElab(int sep, std::string prefix)
{
	this->printPrefix(sep, prefix);
	Channel* channel = this->channel;
	std::string chstr = channel ? this->channel->toString() : "(unbound)";
	TRACE("Port : " << this->getName() << " (\"" << (void*) this << "\"), bounded to channel " << chstr << "\n");
}


std::vector<Process*>* Port::getSensitive(SCElab* elab, bool IsThread)
{
		return elab->getProcessOfPort(this->sc_port, IsThread) ;
}
