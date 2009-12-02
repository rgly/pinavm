#ifndef _PROCESS_HPP
#define _PROCESS_HPP

#include <map>
#include <string>
#include <vector>


#include "llvm/InstrTypes.h"
#include "llvm/Instructions.h"
#include "llvm/BasicBlock.h"
#include "llvm/Function.h"

#include "ElabMember.hpp"

#include "config.h"

using namespace llvm;

struct IRModule;
struct Event;
struct SCCFactory;

struct Process:public ElabMember {
      protected:
	std::string processName;
	IRModule *module;
	 std::string fctName;
	Function *mainFct;
	 std::vector < Event * >events;
	 std::vector < Function * >usedFunctions;

      public:
	 Process(IRModule * mod, Function * fct, std::string name,
		 std::string fctName);
	 std::string getName();
	 std::string getFctName();
	Function *getMainFct();
	IRModule *getModule();
	void addEvent(Event * ev);
	void printIR(SCCFactory * sccfactory);
	void printElab(int sep, std::string prefix);
	 std::vector < Function * >*getUsedFunctions();
	void addUsedFunction(Function * fct);
	bool isFunctionUsed(Function * fct);
};

#endif
