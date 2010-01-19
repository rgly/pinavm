#ifndef _READCONSTRUCT_HPP
#define _READCONSTRUCT_HPP

#include "SCConstruct.hpp"
#include "Port.hpp"

using namespace llvm;

struct ReadConstruct:public SCConstruct {
protected:
	Port * port;
	Value* callInstruction;

public:
	ReadConstruct(Port * p, Value* callInst);
	Value* getCallInst();
	void setPort(Port * e);
	Port *getPort();
	std::string toString();
};

#endif
