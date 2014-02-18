#ifndef _ECHOHELLOWORLD_CPP
#define _ECHOHELLOWORLD_CPP
#include <vector>

#include "Frontend.hpp"
#include "SCElab.h"
#include "Event.hpp"
#include "Process.hpp"
#include "Port.hpp"
#include "Channel.hpp"
#include "IRModule.hpp"

#include "HelloSCCPrinter.h"
#include "HelloConfig.h"

void echo_helloworld(Frontend* fe)
{
  _PRINT("Hello World Backend\n");

  _PRINT("Events:");
  std::vector<Event*>* events = fe->getElab()->getEvents();
  for (unsigned int i = 0; i<events->size(); ++i)
    events->at(i)->printElab(4, "");

  _PRINT("Processes:");
  std::vector<Process*>* processes = fe->getElab()->getProcesses();
  for (unsigned int i = 0; i<processes->size(); ++i){
    processes->at(i)->printElab(4, "");
  }

  _PRINT("Ports:");
  std::vector<Port*>* ports = fe->getElab()->getPorts();
  for (unsigned int i = 0; i<ports->size(); ++i)
    ports->at(i)->printElab(4, "");

  _PRINT("Channels:");
  std::vector<Channel*>* channels = fe->getElab()->getChannels();
  for (unsigned int i = 0; i<channels->size(); ++i) {
    Channel* ch = channels->at(i);
    _PRINT1(ch->toString() << "  type: " << ch->getTypeName());
  }

  _PRINT("SCCs:");
  HelloSCCPrinter printer;
  printer.runOn(fe->getConstructs());
}
#endif
