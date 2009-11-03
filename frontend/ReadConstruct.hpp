#ifndef _READCONSTRUCT_HPP
#define _READCONSTRUCT_HPP

#include "SCConstruct.hpp"
#include "SCPort.hpp"

using namespace llvm;

struct ReadConstruct : public SCConstruct {  
protected :
  SCPort* port;
public:
  ReadConstruct(SCPort* p);
  void setPort(SCPort* e);
  SCPort* getPort();
};

#endif
