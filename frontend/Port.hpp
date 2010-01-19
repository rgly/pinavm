#ifndef _PORT_HPP
#define _PORT_HPP

#include <string>
#include <vector>

#include "llvm/Type.h"

#include "ElabMember.hpp"
#include "Channel.hpp"

using namespace std;
using namespace llvm;

struct IRModule;

struct Port: public ElabMember {
protected:
	IRModule * irModule;
	string name;
	vector<Channel*>* channels;
	Channel* channel;
	channel_id channelID;
	Type* type;

public:
	Port(IRModule * module, string portName);
	IRModule *getModule();
	string getName();
	void printElab(int sep, string prefix);
	vector<Channel*>* getChannels();	
	channel_id getChannelID();
	void addChannel(Channel* ch);
	Type* getType();
	Channel* getChannel();
};

#endif
