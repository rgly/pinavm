#ifndef _FRONTEND_HPP
#define _FRONTEND_HPP

#include <string>

#include "llvm/IR/Module.h"

using namespace llvm;

class SCElab;
class SCCFactory;
class SCJit;
class Process;

extern int launch_systemc(int argc, char *argv[]);

class Frontend {

private:
  SCJit * scjit;
  SCCFactory *sccfactory;
  SCElab *elab;
  Module *llvmMod;
  bool inlineFunctions;
  // if return true, means the function has been inlined.
  bool inlineForProcessFct(Function* F);
  void inlineProcessFct();
  void handleFctConstruct_and_GV(Process* proc, Function* F,
				std::vector<Function*>* fctStack);
  void handleConstruct_and_GV();

public:
   Frontend(Module * M);
  ~Frontend();

  bool run();
  SCElab *getElab();
  void printElab(std::string prefix);
  void printIR();
  SCJit* getJit();

  void setInlineFunctions(bool b);

  Module *getLLVMModule();
  SCCFactory *getConstructs();

  void fillGlobalVars(Instruction* inst);
};

#endif
