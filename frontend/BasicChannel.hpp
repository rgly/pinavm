#ifndef _BASICCHANNEL_HPP
#define _BASICCHANNEL_HPP

#include <string>
#include "Channel.hpp"

class BasicChannel : public Channel
{
public:
	BasicChannel(std::string const &channelName);
	std::string toString();
	
	const std::string getChannelName() const {return m_channelName;}
private:
	const std::string m_channelName;
};

#endif
