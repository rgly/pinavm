#ifndef PROMELABACKENDNAMEALLUSEDSTRUCTSANDMERGEFUNCTIONS_H
#define PROMELABACKENDNAMEALLUSEDSTRUCTSANDMERGEFUNCTIONS_H

#include "PromelaTargetMachine.h"

using namespace llvm;

/// PromelaBackendNameAllUsedStructsAndMergeFunctions - This pass inserts names for
/// any unnamed structure types that are used by the program, and merges
/// external functions with the same name.
///
class PromelaBackendNameAllUsedStructsAndMergeFunctions :  public ModulePass  {
 public:
  static char ID;
 PromelaBackendNameAllUsedStructsAndMergeFunctions() : ModulePass(&ID) {;}
  void getAnalysisUsage(AnalysisUsage &AU) const;
  virtual const char* getPassName() const;
  virtual bool runOnModule(Module &M);
};
#endif
