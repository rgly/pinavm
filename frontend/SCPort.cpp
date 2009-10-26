#include "SCPort.hpp"

Port::Port(SCModule module, string name)
{
  this->moduleName = module;
  this->name = port;
}

SCModule*
Port::getSCModule()
{
  return this->SCModule
}

SCModule*
Port::getBindedModule()
{
  return this->bindedSCModule;
}

void
Port::setBindedModule(SCModule* bindedModule)
{
  this->bindedSCModule = bindedModule;
}

string
Port::getPortName()
{
  return this->name;
}
