#ifndef _PORT_HPP
#define _PORT_HPP

#include <string>
#include <vector>

#include "llvm/IR/Type.h"

#include "ElabMember.hpp"
#include "Channel.hpp"


using namespace llvm;

namespace sc_core {
	class sc_port_base;
}
class SCElab;
class Process;
class IRModule;

class Port: public ElabMember {
protected:
	IRModule * irModule;
	std::string name;
	std::vector<Channel*>* channels;
	Channel* channel;
	channel_id channelID;
	Type* type;

	sc_core::sc_port_base* sc_port;
	std::vector<Process*>* sensitivelist;

	std::vector<Port*>* parents;
public:
	Port(const SCElab* el, IRModule * module, std::string portName, sc_core::sc_port_base* sc_port_);
	~Port();
	IRModule *getModule();
	std::string getName();
	void printElab(int sep, std::string prefix);
	std::vector<Channel*>* getChannels();	
	channel_id getChannelID();
	void addChannel(Channel* ch);
	Type* getType();
	Channel* getChannel();

	// There are two kinds of SystemC processes, Thread and Method.
	std::vector<Process*>* getSensitive(bool IsThread);

	// to keep info for sc_module.port(parent_port) semantic.
	bool hasParent();
	std::vector<Port*>* getParentPorts();
	void addParentPort(Port* port);
};

#endif
