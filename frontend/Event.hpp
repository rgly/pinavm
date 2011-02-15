#ifndef _EVENT_HPP
#define _EVENT_HPP

#include <string>
#include <vector>

#include "ElabMember.hpp"

using namespace std;

struct Process;

struct Event:public ElabMember {
      protected:
	string eventName;
	std::vector<Process*> processes;
	int numEvent;

      public:
	Event(string name);
	string getEventName();
	std::vector<Process*>* getProcesses();
	void printElab(int sep, string prefix);
	int getNumEvent();
	void addProcess(Process* p);
};

#endif
