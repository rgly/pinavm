#ifndef _ECHOHELLOWORLD_CPP
#define _ECHOHELLOWORLD_CPP
#include <iostream>
#include <vector>

#include "Frontend.hpp"
#include "SCElab.h"
#include "Event.hpp"
#include "Process.hpp"
#include "Port.hpp"
#include "Channel.hpp"
#include "IRModule.hpp"

#include "HelloSCCPrinter.h"

#define _PRINT(a) std::cout << a << '\n'
#define _PRINT1(a) std::cout << "\t" << a << '\n'

void echo_helloworld(Frontend* fe)
{
  _PRINT("Hello World Backend\n");

  _PRINT("Events:");
  std::vector<Event*>* events = fe->getElab()->getEvents();
  for (unsigned int i = 0; i<events->size(); ++i)
    _PRINT1(events->at(i)->getEventName());

  _PRINT("Processes:");
  std::vector<Process*>* processes = fe->getElab()->getProcesses();
  for (unsigned int i = 0; i<processes->size(); ++i)
    _PRINT1(processes->at(i)->getName());

  _PRINT("Ports:");
  std::vector<Port*>* ports = fe->getElab()->getPorts();
  for (unsigned int i = 0; i<ports->size(); ++i)
    _PRINT1(ports->at(i)->getName());

  _PRINT("Channels:");
  std::vector<Channel*>* channels = fe->getElab()->getChannels();
  for (unsigned int i = 0; i<channels->size(); ++i)
    _PRINT1(channels->at(i)->getTypeName());

  _PRINT("SCCs:");
  HelloSCCPrinter printer;
  printer.runOn(fe->getConstructs());
}
#endif
