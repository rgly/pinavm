
#include "ZeroTimeConstruct.hpp"


ZeroTimeConstruct::ZeroTimeConstruct():TimeConstruct(0)
{
}

std::string ZeroTimeConstruct::toString()
{
	return "WAIT()";
}
