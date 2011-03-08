#ifndef _BASICCHANNEL_HPP
#define _BASICCHANNEL_HPP

#include <string>
#include "Channel.hpp"

class BasicChannel : public Channel
{
public:
	BasicChannel();
	std::string toString();
};

#endif
