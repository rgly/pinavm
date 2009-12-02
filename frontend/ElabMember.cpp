#include "ElabMember.hpp"
#include "config.h"

#include <string>

using namespace std;

void
 ElabMember::printPrefix(int sep, string prefix)
{
	for (int i = 0; i < sep; i++)
		TRACE(" ");
	TRACE(prefix);
}
