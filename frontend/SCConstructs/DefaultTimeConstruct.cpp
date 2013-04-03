#include <sstream>

#include "DefaultTimeConstruct.hpp"

std::string DefaultTimeConstruct::toString()
{
	std::string s;
	{
		std::ostringstream oss;
		oss << this->time;
		s = oss.str();
	}

	return "WAIT(" + s + ")\n";
}
