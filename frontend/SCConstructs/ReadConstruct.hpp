#ifndef _READCONSTRUCT_HPP
#define _READCONSTRUCT_HPP

#include "SCConstruct.hpp"
class Port;

using namespace llvm;

class ReadConstruct:public SCConstruct {
protected:
	Port * port;
	Value* callInstruction;

public:
	ReadConstruct(Port * p, Value* callInst);
	ReadConstruct(Value* callInst);
	Value* getCallInst();
	void setPort(Port * e);
	Port *getPort();
	std::string toString();
	static inline bool classof(const SCConstruct *scc) {
		return (scc->getID() == READCONSTRUCT );
	}
};

#endif
