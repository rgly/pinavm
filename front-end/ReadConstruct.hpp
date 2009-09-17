#ifndef _READCONSTRUCT_HPP
#define _READCONSTRUCT_HPP

#include "SCConstruct.hpp"
#include "Port.hpp"

using namespace llvm;

struct ReadConstruct : public SCConstruct {  
protected :
  Port* port;
public:
  ReadConstruct(Port* p);
  void setPort(Port* e);
  Port* getPort();
};

#endif
