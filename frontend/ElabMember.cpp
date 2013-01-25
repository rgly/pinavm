#include "ElabMember.hpp"
#include "config.h"

void
ElabMember::printPrefix(int sep, std::string prefix)
{
	for (int i = 0; i < sep; ++i)
		TRACE(" ");
	TRACE(prefix);
}
