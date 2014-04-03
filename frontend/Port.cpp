#include <algorithm>
#include "Port.hpp"
#include "config.h"
#include "SCElab.h"


Port::Port(const SCElab* el, IRModule * module, std::string portName,
		 sc_core::sc_port_base* sc_port_) : ElabMember(el)
{
	this->irModule = module;
	this->name = portName;
	this->channel = NULL;
	this->channels = new std::vector<Channel*>();
	this->channelID = UNDEFINED_CHANNEL;
	this->sc_port = sc_port_ ;
	this->sensitivelist = NULL;
	this->parents = new std::vector<Port*>();
}

Port::~Port()
{
	delete this->sensitivelist;
	delete this->channels;
	delete this->parents;
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
	ch->addPort(this);
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
	TRACE("Port : " << this->getName() << " (\"" << (void*) this << "\"), bounded to :\n");

	const int additional_space = sep + 20;

	std::string str;
	if (this->hasParent()) {
		for(unsigned int i =0; i < this->getParentPorts()->size(); ++i){
			str = this->getParentPorts()->at(i)->getName();
			this->printPrefix(additional_space, prefix);
			TRACE("port " << str << "\n");
		}
	} else {
		str = this->channel ? this->channel->toString() : "(unbound)";
		this->printPrefix(additional_space, prefix);
		TRACE(str << "\n");
	}
}


std::vector<Process*>* Port::getSensitive(bool IsThread)
{
	if (! this->sensitivelist)
		this->sensitivelist = this->getElab()->getSensitive(
						this->sc_port, IsThread) ;
	return this->sensitivelist;
}

bool Port::hasParent()
{
	return (! this->parents->empty());
}

std::vector<Port*>* Port::getParentPorts()
{
	return this->parents;
}

void Port::addParentPort(Port* port)
{
	assert(port);
	if (std::find(this->parents->begin(), this->parents->end(), port) != this->parents->end()) {
		ERROR("Parent Port added twice to the same port : " << port << "\n");
	} else if (this->channelID == UNDEFINED_CHANNEL) {
		this->channelID = port->getChannelID();
		this->type = port->getType();
	} else if (this->channelID != port->getChannelID()) {
		ERROR("Cannot add different types of Parent Port to the same port\n");
	}
	this->parents->push_back(port);
}
