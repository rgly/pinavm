#include "WriteConstruct.hpp"

WriteConstruct::WriteConstruct(SCPort* p)
{
  this->port = p;
}

void
WriteConstruct::setPort(SCPort* p)
{
  this->port = p;
}

SCPort*
WriteConstruct::getPort()
{
  return this->port;
}
