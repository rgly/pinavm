#include "WriteConstruct.hpp"

WriteConstruct::WriteConstruct(Port* p)
{
  this->port = p;
}

void
WriteConstruct::setPort(Port* p)
{
  this->port = p;
}

Port*
WriteConstruct::getPort()
{
  return this->port;
}
