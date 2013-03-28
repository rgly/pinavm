#ifndef PROMELAWRITER_H
#define PROMELAWRITER_H

#include "PromelaTargetMachine.h"

#include "llvm/CallingConv.h"
#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Module.h"
#include "llvm/Instructions.h"
#include "llvm/Pass.h"
#include "llvm/PassManager.h"
#include "llvm/TypeFinder.h"
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
#include "llvm/DataLayout.h"
#include "llvm/Support/CallSite.h"
#include "llvm/Support/CFG.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/GetElementPtrTypeIterator.h"
#include "llvm/Support/InstVisitor.h"
#include "llvm/Target/Mangler.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Config/config.h"

#include <map>
#include <vector>
#include <algorithm>
#include <sstream>
#include <string>
#include <iostream>
#include <utility>

#include "Frontend.hpp"

struct SCConstruct;
struct SCCFactory;
struct SCElab;
struct Process;
struct Event;
struct SCJit;

using namespace llvm;

enum SpecialGlobalClass {
  NotSpecial = 0,
  GlobalCtors, GlobalDtors,
  NotPrinted
};

/// PromelaWriter - This class is the main chunk of code that converts an LLVM
/// module to a "Promela" translation unit.
class PromelaWriter : public ModulePass, public InstVisitor<PromelaWriter> {
  formatted_raw_ostream &Out;
  IntrinsicLowering *IL;
  Mangler *Mang;
  LoopInfo *LI;
  const Module *TheModule;
  /* MM: the C backend uses a derived class from MCAsmInfo. But the
     version of llvm I'm using compiles MCAsmInfo with -fno-rtti while
     we need -frtti (otherwise, systemc.h doesn't compile), and
     deriving a rtti class from a no-rtti doesn't link. For now, let's
     just take MCAsmInfo as it is, we'll play with per-file
     compilation option later if needed.
     */
  const MCAsmInfo *TAsm;
  const DataLayout* TD;
  std::map<Type *, std::string> TypeNames;
  std::map<const ConstantFP *, unsigned> FPConstantMap;
  std::set<Function*> intrinsicPrototypesAlreadyGenerated;
  std::set<const Argument*> ByValParams;
  unsigned FPCounter;
  unsigned OpaqueCounter;
  DenseMap<const Value*, unsigned> AnonValueNumbers;
  unsigned NextAnonValueNumber;
  SCCFactory* sccfactory;
  SCElab* elab;
  SCJit* scjit;
  bool relativeClocks;
  bool eventsAsBool;
  bool insertBug;
  std::set < Type *>StructPrinted;

public:
  static char ID;
  PromelaWriter(Frontend* fe, formatted_raw_ostream &o, bool encodeEventsAsBool, bool useRelativeClocks, bool bug);
  PromelaWriter(formatted_raw_ostream &o);
  const char *getPassName() const;
  void getAnalysisUsage(AnalysisUsage &AU) const;
  bool runOnModule(Module &M);

  raw_ostream &printType(formatted_raw_ostream &Out,
			 Type *Ty, 
			 bool isSigned = false,
			 const std::string &VariableName = "",
			 bool IgnoreName = false,
			 const AttrListPtr &PAL = AttrListPtr());
  std::ostream &printType(std::ostream &Out, Type *Ty, 
			  bool isSigned = false,
			  const std::string &VariableName = "",
			  bool IgnoreName = false,
			  const AttrListPtr &PAL = AttrListPtr());
  raw_ostream &printSimpleType(formatted_raw_ostream &Out,
			       Type *Ty, 
			       bool isSigned, 
			       const std::string &NameSoFar = "");
  std::ostream &printSimpleType(std::ostream &Out, Type *Ty, 
				bool isSigned, 
				const std::string &NameSoFar = "");

  void printStructReturnPointerFunctionType(formatted_raw_ostream &Out,
					    const AttrListPtr &PAL,
					    PointerType *Ty);
 /* raw_ostream &printInitialValue(formatted_raw_ostream &Out,
			Type * Ty,
			bool isSigned, const std::string & NameSoFar = "");*/
  /// writeOperandDeref - Print the result of dereferencing the specified
  /// operand with '*'.  This is equivalent to printing '*' then using
  /// writeOperand, but avoids excess syntax in some cases.
  void writeOperandDeref(Value *Operand);
    
  void writeOperand(Value *Operand, bool Static = false);
  void writeInstComputationInline(Instruction &I);
  void writeOperandInternal(Value *Operand, bool Static = false);
  void writeOperandWithCast(Value* Operand, unsigned Opcode);
  void writeOperandWithCast(Value* Operand, const ICmpInst &I);
  bool writeInstructionCast(const Instruction &I);

  void writeMemoryAccess(Value *Operand, Type *OperandType,
			 bool IsVolatile, unsigned Alignment);

  private :
    std::string InterpretASMConstraint(InlineAsm::ConstraintInfo& c);

  void lowerIntrinsics(Function &F);

  void printModule(Module *M);
  void printModuleTypes(Module &M);
  bool fillContainedStructs(Type *Ty, bool fill);
  void printFloatingPointConstants(Function &F);
  void printFloatingPointConstants(const Constant *C);
  void printFunctionSignature(const Function *F, bool Prototype, bool inlineFct);
  void printReturnVariables(const Function* F);

  void printFunction(Function &, bool inlineFct);
  void printBasicBlock(BasicBlock *BB);

  void printCast(unsigned opcode, Type *SrcTy, Type *DstTy);
  void printConstant(Constant *CPV, bool Static);
  void printConstantWithCast(Constant *CPV, unsigned Opcode);
  bool printConstExprCast(const ConstantExpr *CE, bool Static);
  void printConstantArray(ConstantArray *CPA, bool Static);
  void printConstantVector(ConstantVector *CV, bool Static);

  /// isAddressExposed - Return true if the specified value's name needs to
  /// have its address taken in order to get a C value of the correct type.
  /// This happens for global variables, byval parameters, and direct allocas.
  bool isAddressExposed(const Value *V) const;
        
  // Instruction visitation functions
  friend class InstVisitor<PromelaWriter>;
  
  void visitSystemCStruct(Instruction &I);
  void visitReturnInst(ReturnInst &I);
  void visitBranchInst(BranchInst &I);
  void visitSwitchInst(SwitchInst &I);
  void visitInvokeInst(InvokeInst &I);
// not supported by LLVM 3.2 , so remove.
//  void visitUnwindInst(UnwindInst &I);
  void visitUnreachableInst(UnreachableInst &I);

  void visitPHINode(PHINode &I);
  void visitBinaryOperator(Instruction &I);
  void visitICmpInst(ICmpInst &I);
  void visitFCmpInst(FCmpInst &I);

  void visitCastInst (CastInst &I);
  void visitSelectInst(SelectInst &I);

  void printRandFct();
  void printCodingGlobals();
  void printSelectClock();
  void printWaitTimePrimitive();
  void printNotifyPrimitive();
  void printWaitEventPrimitive();
  void printPrimitives();
  void printGlobalVariables(Mangler* mang);
  void printProcesses();
  void printInitSection();

  std::string getEventName(Process* proc, Event* event);
  void visitSCConstruct(SCConstruct * scc);
  void visitCallInst (CallInst &I);
  void visitInlineAsm(CallInst &I);
  bool visitBuiltinCall(CallInst &I, Intrinsic::ID ID, bool &WroteCallee);

	// MM: malloc and free are now normal functions.
//  void visitMallocInst(MallocInst &I);
  void visitAllocaInst(AllocaInst &I);
//  void visitFreeInst  (FreeInst   &I);
  void visitLoadInst  (LoadInst   &I);
  void visitStoreInst (StoreInst  &I);
  void visitGetElementPtrInst(GetElementPtrInst &I);
  void visitVAArgInst (VAArgInst &I);

  void visitInsertElementInst(InsertElementInst &I);
  void visitExtractElementInst(ExtractElementInst &I);
  void visitShuffleVectorInst(ShuffleVectorInst &SVI);

  void visitInsertValueInst(InsertValueInst &I);
  void visitExtractValueInst(ExtractValueInst &I);
  void visitFenceInst(FenceInst &I);

  void visitInstruction(Instruction &I);

  void outputLValue(Instruction *I);

  bool printPHICopiesForSuccessor(BasicBlock *CurBlock,
				  BasicBlock *Successor, unsigned Indent);
  void printBranchToBlock(BasicBlock *CurBlock, BasicBlock *SuccBlock,
			  unsigned Indent);
  void printGEPExpression(Value *Ptr, gep_type_iterator I,
			  gep_type_iterator E, bool Static);

  std::string GetValueName(const Value *Operand);
  
  void fillDependencies(const Function* F,
		  std::string prefix,
		  std::vector<std::pair<std::string, Type*> >* args,
		  std::vector<std::pair<std::string, Type*> >* ret);
  
  void insertAllFields(std::vector<std::pair<std::string, Type*> >* deps,
		  std::map<std::string, Type*>* allDepsByName,
		  std::string parentName,
		  StructType* structType);
  void addVectors(std::vector<std::pair<std::string, Type*> >* from,
		  std::vector<std::pair<std::string, Type*> >* to);
  bool isTypeEmpty(Type* ty);
  bool isSystemCStruct(StructType* ty);
  bool isSystemCType(Type* ty);
  void getValueDependencies(Value* value,
			  std::string prefix,
			  std::vector<std::pair<std::string, Type*> >* args,
			  std::vector<std::pair<std::string, Type*> >* ret,
			  std::map<Value*, std::string>* allDepsByValue,
			  std::map<std::string, Type*>* allDepsByName);
  int getNumField(GetElementPtrInst* inst);
  int getTypeNamesSize();
  void insertTypeName(Type* Ty, std::string TyName);
  Attributes getAttributes(Attributes::AttrVal attr);


};

#endif
