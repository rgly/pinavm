#ifndef _WRITECONSTRUCT_HPP
#define _WRITECONSTRUCT_HPP

#include "SCConstruct.hpp"
#include "Port.hpp"

using namespace llvm;

struct WriteConstruct: public SCConstruct {

protected:
	Port * port;
	string value;

public:
	WriteConstruct(Port * p, string valueAsString);
	WriteConstruct(Port * p, Value* missingValue);
	std::string getValue();
	void setPort(Port * e);
	Port *getPort();
	std::string toString();
};

#endif
