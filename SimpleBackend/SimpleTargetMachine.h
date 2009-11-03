#ifndef SIMPLETARGETMACHINE_H
#define SIMPLETARGETMACHINE_H


#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetData.h"

#include "SimpleBackendNameAllUsedStructsAndMergeFunctions.h"
#include "SimpleWriter.h"

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
