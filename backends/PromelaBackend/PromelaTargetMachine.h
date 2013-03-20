#ifndef PROMELATARGETMACHINE_H
#define PROMELATARGETMACHINE_H


#include "llvm/Target/TargetMachine.h"
#include "llvm/DataLayout.h"
#include "llvm/PassManager.h"

namespace llvm {
  
  struct PromelaTargetMachine : public TargetMachine {
  PromelaTargetMachine(const Target &T, StringRef TargetTriple, StringRef CPU,
                    StringRef FS, const TargetOptions &Options, Reloc::Model RM,
                    CodeModel::Model CM, CodeGenOpt::Level OL)
    : TargetMachine(T, TargetTriple, CPU, FS, Options ) {}
    
    virtual bool WantsWholeFile() const {
      return true;
    }
    virtual bool addPassesToEmitWholeFile(PassManager &PM,
					  formatted_raw_ostream &Out,
					  CodeGenFileType FileType,
					  CodeGenOpt::Level OptLevel,
					  bool DisableVerify = true);

    virtual const DataLayout*
    getDataLayout() const {
      return 0;
    }

  };
  
  extern Target ThePromelaBackendTarget;
  
} // End llvm namespace


#endif
