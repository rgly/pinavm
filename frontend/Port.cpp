#include "Port.hpp"

Port::Port(IRModule* module, string portName)
{
  this->irModule = module;
  this->name = portName;
}

IRModule*
Port::getModule()
{
  return this->irModule;
}

string
Port::getName()
{
  return this->name;
}
