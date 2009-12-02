#ifndef _EVENT_HPP
#define _EVENT_HPP

#include <string>

#include "ElabMember.hpp"

using namespace std;

struct Process;

struct Event:public ElabMember {
      protected:
	string eventName;
	Process *process;

      public:
	 Event(Process * p, string eventName);
	string toString();
	Process *getProcess();
	void printElab(int sep, string prefix);
};

#endif
