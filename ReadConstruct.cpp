#include "ReadConstruct.hpp"

ReadConstruct::ReadConstruct(Port* p)
{
  this->port = p;
}

void
ReadConstruct::setPort(Port* p)
{
  this->port = p;
}

Port*
ReadConstruct::getPort()
{
  return this->port;
}
