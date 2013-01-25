#include <sstream>
#include "BasicChannel.hpp"

BasicChannel::BasicChannel(std::string const &channelName) 
: Channel(NULL, "BasicChannel"), m_channelName(channelName)
{
	this->id = BASIC_CHANNEL;
}

std::string
BasicChannel::toString()
{
	std::stringstream ss;
	ss << "BasicChannel " << (void*) this;
	return ss.str();
}
