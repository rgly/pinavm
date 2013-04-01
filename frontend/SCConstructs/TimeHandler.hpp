#ifndef _TIMEHANDLER_HPP
#define _TIMEHANDLER_HPP

#include "SCConstructHandler.hpp"

struct SCJit;

class TimeHandler:public SCConstructHandler {
      protected:
	int time;

      public:
	 TimeHandler(SCJit * jit):SCConstructHandler(jit) {}
	int getTime();
};

#endif
