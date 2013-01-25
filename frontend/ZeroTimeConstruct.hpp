#ifndef _ZEROTIMECONSTRUCT_HPP
#define _ZEROTIMECONSTRUCT_HPP

#include "TimeConstruct.hpp"


struct ZeroTimeConstruct:public TimeConstruct {
      public:
	ZeroTimeConstruct();
	std::string toString();
};

#endif
