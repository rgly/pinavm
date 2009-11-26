#ifndef _SCCONSTRUCTHANDLER_HPP
#define _SCCONSTRUCTHANDLER_HPP

#include <map>
#include <string>

#include "llvm/Support/raw_ostream.h"

#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "config.h"

using namespace llvm;

struct SCJit;
struct SCConstruct;

class SCConstructHandler {
protected:
  SCJit * scjit;
  
public:
  SCConstructHandler(SCJit * jit);
  SCConstructHandler();
  virtual SCConstruct *handle(llvm::Function * fct, BasicBlock * bb,
			      CallInst * callInst) = 0;
  void insertInMap(std::map < Function *, SCConstructHandler * >* scchandlers,
		   std::string fctName);
};

#endif
