/*
#ifndef SIMPLEBACKENDNAMEALLUSEDSTRUCTSANDMERGEFUNCTIONS_H
#define SIMPLEBACKENDNAMEALLUSEDSTRUCTSANDMERGEFUNCTIONS_H

#include "SimpleTargetMachine.h"

using namespace llvm;

/// SimpleBackendNameAllUsedStructsAndMergeFunctions - This pass inserts names for
/// any unnamed structure types that are used by the program, and merges
/// external functions with the same name.
///
class SimpleBackendNameAllUsedStructsAndMergeFunctions :  public ModulePass  {
 public:
  static char ID;
 SimpleBackendNameAllUsedStructsAndMergeFunctions() : ModulePass(ID) {;}
  void getAnalysisUsage(AnalysisUsage &AU) const;
  virtual const char* getPassName() const;
  virtual bool runOnModule(Module &M);
};
#endif
*/
