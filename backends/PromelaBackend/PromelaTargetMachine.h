#ifndef PROMELATARGETMACHINE_H
#define PROMELATARGETMACHINE_H


#include "llvm/Target/TargetMachine.h"
#include "llvm/DataLayout.h"

#include "PromelaBackendNameAllUsedStructsAndMergeFunctions.h"

namespace llvm {
  
  struct PromelaTargetMachine : public TargetMachine {
  PromelaTargetMachine(const Target &T, const std::string &TT, const std::string &FS)
    : TargetMachine(T) {}
    
    virtual bool WantsWholeFile() const {
      return true;
    }
    virtual bool addPassesToEmitWholeFile(PassManager &PM,
					  formatted_raw_ostream &Out,
					  CodeGenFileType FileType,
					  CodeGenOpt::Level OptLevel,
					  bool DisableVerify = true);

    virtual const DataLayout*
    getTargetData() const {
      return 0;
    }

  };
  
  extern Target ThePromelaBackendTarget;
  
} // End llvm namespace


#endif
