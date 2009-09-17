#ifndef _SCCONSTRUCTHANDLER_HPP
#define _SCCONSTRUCTHANDLER_HPP

#include "SCConstruct.hpp"

using namespace llvm;

struct Field {
protected:
  SCConstructHandler(const char* functionName, Module& mdl, SCJit* jit);
  Function* handledFct;
  
public:
  Field(char* moduleName, char* className, char*);
  Function* getHandledFct();  
  void setHandledFct(Function* fct);
  void initMap(const char* fctName,
	       std::map<Function*, SCConstructHandler*> scchandlers,
	       Module &mdl);
  SCConstruct* handle(llvm::Function* fct, BasicBlock* bb, CallInst* callInst);
};

#endif
