#ifndef _PROCESS_HPP
#define _PROCESS_HPP

#include <string>
#include <vector>


#include "llvm/Function.h"

#include "ElabMember.hpp"


struct IRModule;
struct Event;
struct Port;
struct SCCFactory;

struct Process:public ElabMember {
protected:
    std::string processName;
    IRModule *module;
    std::string fctName;
    llvm::Function *mainFct;
    std::vector < Event * > events;
    std::vector < Port * > ports;
    std::vector < llvm::Function * > usedFunctions;
    int pid;

public:
    Process(IRModule * mod, llvm::Function * fct, std::string name,
             std::string fctName);
    std::string getName();
    std::string getFctName();
    llvm::Function *getMainFct();
    IRModule *getModule();
    void addEvent(Event * ev);
    void printIR(SCCFactory * sccfactory);
    void printElab(int sep, std::string prefix);
    std::vector < llvm::Function * >*getUsedFunctions();
    std::vector < Event* >* getEvents();
    void addUsedFunction(llvm::Function * fct);
    int getPid();
};

#endif
