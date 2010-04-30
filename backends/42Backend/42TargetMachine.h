#ifndef _42TARGETMACHINE_H
#define _42TARGETMACHINE_H


#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetData.h"

#include "42BackendNameAllUsedStructsAndMergeFunctions.h"

namespace llvm {
  
  struct _42TargetMachine : public TargetMachine {
  _42TargetMachine(const Target &T, const std::string &TT, const std::string &FS)
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
  
  extern Target The42BackendTarget;
  
} // End llvm namespace


#endif
