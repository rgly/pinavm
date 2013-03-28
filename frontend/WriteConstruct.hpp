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
	std::string value;

public:
	WriteConstruct(Port * p);
	WriteConstruct(Value * p);
	std::string getValue();
	void setMissingValue(Value* mv);
	void setValue(std::string sv);
	Value * getMissingValue();
	Value * getMissingPort();
	void setPort(Port * e);
	Port *getPort();
	std::string toString();
	bool isValueDefined();
	bool isPortDefined();
};

#endif
