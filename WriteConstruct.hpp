#ifndef _WRITECONSTRUCT_HPP
#define _WRITECONSTRUCT_HPP

#include "SCConstruct.hpp"
#include "Port.hpp"

using namespace llvm;

struct WriteConstruct : public SCConstruct {  
protected :
  Port* port;
public:
  WriteConstruct(Port* p);
  void setPort(Port* e);
  Port* getPort();
};

#endif
