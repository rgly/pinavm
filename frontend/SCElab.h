// -*- c-basic-offset: 2 -*-
#ifndef _SCELAB_H
#define _SCELAB_H


#include <map>
#include <vector>

#include "llvm/IR/Module.h"


#include "ElabMember.hpp"

// PinaVM frondend classes.
class SCCFactory;
class IRModule;
class Process;
class Port;
class Event;
class Channel;
struct Bus;

namespace sc_core {
  class sc_module;
  class sc_process_b;
  class sc_port_base;
  class sc_event;
  class sc_interface;
}


using namespace llvm;

class SCElab : public ElabMember
{
 private:
  std::map<sc_core::sc_module*, IRModule*> modulesMap;
  std::map<sc_core::sc_process_b*, Process*> processMap;
  std::map<sc_core::sc_port_base*, Port*> portsMap;
  std::map<sc_core::sc_event*, Event*> eventsMap;
  std::map<sc_core::sc_interface*, Channel*> channelsMap;
  std::map<Channel* , Bus*> busMap;

  std::map<IRModule*, sc_core::sc_module*> ir2scModules;
  
  std::vector<IRModule*> modules;
  std::vector<Process*> processes;
  std::vector<Port*> ports;
  std::vector<Event*> events;
  std::vector<Channel*> channels;

  std::vector<GlobalValue*> globalVariables;

  llvm::Module* llvmMod;

 public:
  SCElab(Module* llvmModule); 
  ~SCElab();
   
  IRModule* addModule(sc_core::sc_module* mod);
  Process* addProcess(IRModule* mod, sc_core::sc_process_b* process);
  Port* addPort(IRModule* mod, sc_core::sc_port_base* port);
  Event* addEvent(Process* process, sc_core::sc_event* event);

  IRModule* getIRModule(void* moduleAddr);
  Process* getProcess(void* processAddr);
  Port* getPort(void* portAddr);
  Event* getEvent(void* eventAddr);
  Channel* getChannel(void* channelAddr);
  Bus* getBus(Channel *chan);
    
    
  void printElab(int sep, std::string prefix);

  sc_core::sc_module* getSCModule(IRModule* irmod);

  int getNumEvents();
  int getNumProcesses();
  std::vector<Process*>* getProcesses();
  std::vector<Port*>* getPorts();
  std::vector<Channel*>* getChannels();
  std::vector<Event*>* getEvents();
  std::vector<IRModule*>* getIRModules();

  void addGlobalVariable(GlobalValue *globalVar);
  std::vector < GlobalValue * >* getGlobalVariables();

  void addProcessAndEvents(sc_core::sc_process_b *theProcess, sc_core::sc_module * mod);
  void complete();
  std::vector<Process*>* getSensitive(const sc_core::sc_port_base*, bool) const; 

private:
  Port * tryParentPort(IRModule * mod, std::string portName,
			sc_core::sc_port_base * port);
  Port * trySc_Signal(IRModule * mod,
		      sc_core::sc_interface* itf, std::string &itfTypeName,
		      sc_core::sc_port_base * port, std::string portName);
  Port * trySc_Clock(IRModule * mod,
		     sc_core::sc_interface* itf, std::string &itfTypeName,
		     sc_core::sc_port_base * port, std::string portName);
  Port * tryBasicTarget(IRModule * mod, 
                        sc_core::sc_interface* itf, std::string &itfTypeName,
			sc_core::sc_port_base * port, std::string portName);
  Port * tryBasicInitiator(IRModule * mod, 
			   sc_core::sc_interface* itf, std::string &itfTypeName,
			   sc_core::sc_port_base * port, std::string portName);
  std::string getBasicChannelName(sc_core::sc_interface* itf);
};

#endif
