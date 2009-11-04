#include "ReadConstruct.hpp"

ReadConstruct::ReadConstruct(SCPort * p)
{
	this->port = p;
}

void
 ReadConstruct::setPort(SCPort * p)
{
	this->port = p;
}

SCPort *ReadConstruct::getPort()
{
	return this->port;
}
