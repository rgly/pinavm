#ifndef _PORT_HPP
#define _PORT_HPP

#include <string>
#include <vector>

#include "llvm/Type.h"

#include "ElabMember.hpp"
#include "Channel.hpp"
#include "Process.hpp"
#include "SCElab.h"

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

	sc_core::sc_port_base* sc_port;

public:
	Port(IRModule * module, string portName, sc_core::sc_port_base* sc_port_);
	IRModule *getModule();
	string getName();
	void printElab(int sep, string prefix);
	vector<Channel*>* getChannels();	
	channel_id getChannelID();
	void addChannel(Channel* ch);
	Type* getType();
	Channel* getChannel();

	// there are two types of process, Thread and Method.
	vector<Process*>* getSensitive(SCElab* elab, bool IsThread);
};

#endif
