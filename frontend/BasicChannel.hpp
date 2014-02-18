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
	static inline bool classof(const Channel *ch) {
		return (ch->getID() == BASIC_CHANNEL);
	}
private:
	const std::string m_channelName;
};

#endif
