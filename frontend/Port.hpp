#ifndef _PORT_HPP
#define _PORT_HPP

#include <string>
#include <vector>

#include "llvm/Type.h"

#include "ElabMember.hpp"
#include "Channel.hpp"


using namespace llvm;

namespace sc_core{struct sc_port_base;}
struct SCElab;
struct Process;
struct IRModule;

struct Port: public ElabMember {
protected:
	IRModule * irModule;
	std::string name;
	std::vector<Channel*>* channels;
	Channel* channel;
	channel_id channelID;
	Type* type;

	sc_core::sc_port_base* sc_port;

public:
	Port(IRModule * module, std::string portName, sc_core::sc_port_base* sc_port_);
	IRModule *getModule();
	std::string getName();
	void printElab(int sep, std::string prefix);
	std::vector<Channel*>* getChannels();	
	channel_id getChannelID();
	void addChannel(Channel* ch);
	Type* getType();
	Channel* getChannel();

	// There are two kinds of SystemC processes, Thread and Method.
	std::vector<Process*>* getSensitive(SCElab* elab, bool IsThread);
};

#endif
