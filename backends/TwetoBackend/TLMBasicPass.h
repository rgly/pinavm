/**
 * TLMBasicPass.h
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : si-mohamed.lamraoui@imag.fr
 * @date : 2011/03/17
 * @copyright : Verimag 2011
 */

#ifndef TLMBASICPASS_H
#define TLMBASICPASS_H

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
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetRegistry.h"
#include "llvm/Support/CallSite.h"
#include "llvm/Support/CFG.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/GetElementPtrTypeIterator.h"
#include "llvm/Support/InstVisitor.h"
#include "llvm/Target/Mangler.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/System/Host.h"
#include "llvm/Config/config.h"

#include <map>
#include <vector>
#include <algorithm>
#include <sstream>
#include <string>
#include <iostream>
#include <utility>

#include "Frontend.hpp"
#include "IRModule.hpp"

struct SCConstruct;
struct SCCFactory;
struct SCElab;
struct Process;
struct Event;
struct SCJit;

using namespace llvm;


//============================================================================
class TLMBasicPass : public ModulePass {

    
    private:
        Frontend *fe;
        SCElab* elab;

    public:
        static char ID;
        TLMBasicPass(Frontend *fe);
        bool runOnModule(Module &M);
    
    private:
        Function* lookForWriteFunction(IRModule *module);
        Function* lookForReadFunction(IRModule *module);
        int replaceCallsInProcess(Process *proc, 
                    Function *writef, Function *readf);

};
//============================================================================


#endif