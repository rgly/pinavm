#ifndef _WRITECONSTRUCT_HPP
#define _WRITECONSTRUCT_HPP

#include "SCConstruct.hpp"
#include "SCPort.hpp"

using namespace llvm;

struct WriteConstruct : public SCConstruct {  
protected :
  SCPort* port;
public:
  WriteConstruct(SCPort* p);
  void setPort(SCPort* e);
  SCPort* getPort();
};

#endif
