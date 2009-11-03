#include "SCPort.hpp"

SCPort::SCPort(SCModule* module, string portName)
{
  this->scModule = scModule;
  this->name = portName;
}

SCModule*
SCPort::getSCModule()
{
  return this->scModule;
}

SCModule*
SCPort::getBindedModule()
{
  return this->bindedSCModule;
}

void
SCPort::setBindedModule(SCModule* bindedModule)
{
  this->bindedSCModule = bindedModule;
}

string
SCPort::getPortName()
{
  return this->name;
}
