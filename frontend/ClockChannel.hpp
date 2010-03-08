#ifndef _CLOCKCHANNEL_HPP
#define _CLOCKCHANNEL_HPP

#include <string>
#include "Channel.hpp"

class ClockChannel : public Channel
{
public:
	ClockChannel();
	std::string toString();
};

#endif
