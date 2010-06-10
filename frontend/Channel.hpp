#ifndef _CHANNEL_HPP
#define _CHANNEL_HPP

#include <string.h>
#include <vector>
#include <llvm/Type.h>

struct Port;

using namespace llvm;

typedef enum {
	UNDEFINED_CHANNEL,
	SIMPLE_CHANNEL,
	FORWARDING_CHANNEL,
	FIFO_CHANNEL,
	CLOCK_CHANNEL
} channel_id;

class Channel {
protected:
	channel_id id;
	std::vector<Port*>* ports;
	Type* type;
	std::string chTypeName;

public:
	Channel(Type* t, std::string typeName);
	std::vector<Port*>* getPorts();
	channel_id getID();
	Type* getType();
	std::string getTypeName();
	virtual std::string toString() = 0;
};


#endif
