#ifndef SIMPLEBACKENDNAMEALLUSEDSTRUCTSANDMERGEFUNCTIONS_H
#define SIMPLEBACKENDNAMEALLUSEDSTRUCTSANDMERGEFUNCTIONS_H

#include "SimpleTargetMachine.h"
#include "llvm/CallingConv.h"
#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Module.h"
#include "llvm/Instructions.h"
#include "llvm/Pass.h"
#include "llvm/PassManager.h"
#include "llvm/TypeSymbolTable.h"
#include "llvm/Intrinsics.h"
#include "llvm/IntrinsicInst.h"
#include "llvm/InlineAsm.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Analysis/ConstantsScanner.h"
#include "llvm/Analysis/FindUsedTypes.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/IntrinsicLowering.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetRegistry.h"
#include "llvm/Support/CallSite.h"
#include "llvm/Support/CFG.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/GetElementPtrTypeIterator.h"
#include "llvm/Support/InstVisitor.h"
#include "llvm/Support/Mangler.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/System/Host.h"
#include "llvm/Config/config.h"
#include <algorithm>
#include <sstream>
using namespace llvm;

/// SimpleBackendNameAllUsedStructsAndMergeFunctions - This pass inserts names for
/// any unnamed structure types that are used by the program, and merges
/// external functions with the same name.
///
class SimpleBackendNameAllUsedStructsAndMergeFunctions : public ModulePass {
 public:
  static char ID;
  SimpleBackendNameAllUsedStructsAndMergeFunctions() : ModulePass(&ID) {}
  void getAnalysisUsage(AnalysisUsage &AU) const;  

  virtual const char* getPassName() const {
    return "Simple backend type canonicalizer";
  };
  virtual bool runOnModule(Module &M);
};

#endif
