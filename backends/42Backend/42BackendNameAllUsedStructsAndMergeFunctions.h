/*
#ifndef _42BACKENDNAMEALLUSEDSTRUCTSANDMERGEFUNCTIONS_H
#define _42BACKENDNAMEALLUSEDSTRUCTSANDMERGEFUNCTIONS_H

#include "42TargetMachine.h"

using namespace llvm;

/// 42BackendNameAllUsedStructsAndMergeFunctions - This pass inserts names for
/// any unnamed structure types that are used by the program, and merges
/// external functions with the same name.
///
class _42BackendNameAllUsedStructsAndMergeFunctions :  public ModulePass  {
 public:
  static char ID;
 _42BackendNameAllUsedStructsAndMergeFunctions() : ModulePass(ID) {;}
  void getAnalysisUsage(AnalysisUsage &AU) const;
  virtual const char* getPassName() const;
  virtual bool runOnModule(Module &M);
};
#endif
*/
