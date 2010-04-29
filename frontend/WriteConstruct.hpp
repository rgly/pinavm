#ifndef _WRITECONSTRUCT_HPP
#define _WRITECONSTRUCT_HPP

#include "SCConstruct.hpp"
#include "Port.hpp"

using namespace llvm;

struct WriteConstruct: public SCConstruct {

protected:
	Port * port;
	Value * missingValue;
	Value * missingPort;
	string value;

public:
	WriteConstruct(Port * p, string valueAsString);
	WriteConstruct(Port * p, Value* missingValue);
	WriteConstruct(Value* missingP, Value* missingV);
	std::string getValue();
	Value * getMissingValue();
	Value * getMissingPort();
	void setPort(Port * e);
	Port *getPort();
	std::string toString();
};

#endif
