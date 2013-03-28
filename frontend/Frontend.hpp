#ifndef _FRONTEND_HPP
#define _FRONTEND_HPP

#include <string>

#include "llvm/Module.h"

using namespace llvm;

struct SCElab;
struct SCCFactory;
struct SCJit;
struct Process;

extern int launch_systemc(int argc, char *argv[]);

class Frontend {

private:
  SCJit * scjit;
  SCCFactory *sccfactory;
  SCElab *elab;
  Module *llvmMod;
  bool inlineFunctions;

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
