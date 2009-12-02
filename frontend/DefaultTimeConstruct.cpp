#include <iostream>
#include <sstream>

#include "DefaultTimeConstruct.hpp"

using namespace llvm;

string DefaultTimeConstruct::toString()
{
	std::string s;
	{
		std::ostringstream oss;
		oss << this->time;
		s = oss.str();
	}

	return "WAIT(" + s + ")\n";
}
