#ifndef SIMPLETARGETMACHINE_H
#define SIMPLETARGETMACHINE_H

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
#include "llvm/Target/TargetMachine.h"
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

#include "SimpleBackendNameAllUsedStructsAndMergeFunctions.h"
#include "SimpleWriter.h"

using namespace llvm;

namespace llvm {
  
  struct SimpleTargetMachine : public TargetMachine {
    SimpleTargetMachine(const Target &T, const std::string &TT, const std::string &FS)
      : TargetMachine(T) {}
    
    virtual bool WantsWholeFile() const {
      return true;
    }
    virtual bool addPassesToEmitWholeFile(PassManager &PM,
					  formatted_raw_ostream &Out,
					  CodeGenFileType FileType,
					  CodeGenOpt::Level OptLevel);
    
    virtual const TargetData*
    getTargetData() const {
      return 0;
    }

  };
  
  extern Target TheSimpleBackendTarget;
  
} // End llvm namespace


#endif
