/**
 * TwetoSpecialize.cpp
 *
 * 
 *
 * @author : Claude Helmstetter, Si-Mohamed Lamraoui
 * @contact : si-mohamed.lamraoui@imag.fr
 * @date : 2011/03/01
 * @copyright : LIAMA 2010, Verimag 2011
 */

#define DEBUG_TYPE "tweto"

#include "TwetoSpecialize.h"
#include "TwetoConstMemory.h"

#include <llvm/ADT/Statistic.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/DerivedTypes.h>
#include <llvm/LLVMContext.h>
#include <llvm/PassManager.h>
#include <llvm/Support/CallSite.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/IRBuilder.h>
#include <llvm/Support/InstIterator.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/DataLayout.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#include <llvm/Transforms/Utils/Cloning.h>
#include <iostream>

using namespace llvm;

STATISTIC(TotalConstLoad, "Number of constant memory loads pre-executed");

/*namespace {
  cl::opt<bool>
  PrintEach("print-each",
            cl::desc("Print the specialized functions after each change"),
            cl::init(false));

  cl::opt<bool>
  PrintAllSpecialized("print-spec-funs",
                      cl::desc("Print the specialized functions"),
                      cl::init(false));

  cl::opt<bool>
  NoTwetoOpt("disable-tweto-opt",
             cl::desc("Disable all tweto optimizations"),
             cl::init(false));
}*/
static bool PrintEach = false;
static bool PrintAllSpecialized = false;
static bool NoTwetoOpt = false;


static IRBuilder<> *IRB = NULL;
static FunctionPassManager *FPM = NULL;
static DataLayout *TD = NULL;
static IntegerType *i64;
static IntegerType *i32;
static IntegerType *i16;
static IntegerType *i8;
static IntegerType *i_ptr;

static int execute_constant_loads(Function*);
static int resolve_indirect_calls(Function*, Module*, ExecutionEngine*);
static int specialize_calls(ExecutionEngine*, Module*, Function*);

void tweto_specialize__create(Module *Mod, Function *oldfunc,
                              Value **args_begin, Value **args_end, bool &already,
                              Function *&newfunc, CallInst *&ci);
void tweto_specialize__optimize(Function *newfunc, CallInst *ci,
                                ExecutionEngine *EE, Module *Mod, Function *oldfunc,
                                Value **args_begin, Value **args_end);

struct ltstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};

typedef std::map<const char*,Function*,ltstr> specfun_map;
typedef std::pair<const char*,Function*> specfun_pair;

static specfun_map spec_functions = specfun_map();


/**
 * init
 *
 */
static void init(Module *Mod) {
  static bool done = false;
  if (done) return;
  LLVMContext &Context = getGlobalContext();
  IRB = new IRBuilder<>(Context);
  TD = new DataLayout(Mod);
  FPM = new FunctionPassManager(Mod);
  FPM->add(TD);
  FPM->add(createIndVarSimplifyPass());
  FPM->add(createLoopUnrollPass());
  FPM->add(createInstructionCombiningPass());
  FPM->add(createReassociatePass());
  FPM->add(createGVNPass());
  FPM->add(createCFGSimplificationPass());
  i64 = Type::getInt64Ty(Context);
  i32 = Type::getInt32Ty(Context);
  i16 = Type::getInt16Ty(Context);
  i8 = Type::getInt8Ty(Context);
  i_ptr = (sizeof(void*)==8)? i64: i32;
  done = true;
}


/**
 * tweto_specialize
 *
 *
 */
Function *tweto_specialize(ExecutionEngine *EE, Module *Mod, Function *oldfunc,
                           Value **args_begin, Value **args_end) {
  bool already = false;
  CallInst *ci = NULL;
  Function *newfunc = NULL;
  tweto_specialize__create(Mod, oldfunc, args_begin, args_end, already, newfunc, ci);
  assert(newfunc);
  if (!already && !NoTwetoOpt) {
    assert(ci);
    tweto_specialize__optimize(newfunc, ci, EE, Mod, oldfunc, args_begin, args_end);
  }
  return newfunc;
}


/**
 * tweto_specialize__create
 *
 *
 */
void tweto_specialize__create(Module *Mod, Function *oldfunc,
                              Value **args_begin, Value **args_end, bool &already,
                              Function *&newfunc, CallInst *&ci) {
  // std::cerr <<"tweto: specialize the function: " <<oldfunc->getNameStr() <<".\n";
  init(Mod);
  formatted_raw_ostream *Out = &fouts();
  LLVMContext &Context = getGlobalContext();
  const FunctionType *oldfunc_type = oldfunc->getFunctionType();
  const unsigned args_size = args_end-args_begin;
  Value **args = args_begin;
  assert(oldfunc_type->getNumParams()==args_size);
  assert(!oldfunc->isDeclaration());

  // compute the type of the new function
  std::vector<Type*> arg_types;
  for (unsigned i = 0; i!=args_size; ++i)
    if (args[i]==NULL)
      arg_types.push_back(oldfunc_type->getParamType(i));
  FunctionType *newfunc_type =
    FunctionType::get(oldfunc_type->getReturnType(), arg_types, false);

  // create the new function
  std::string name = oldfunc->getName().str()+std::string("_specialized");
  newfunc = Function::Create(newfunc_type, Function::ExternalLinkage, StringRef(name), Mod);
  assert(newfunc->empty());
  newfunc->addFnAttr(Attributes::InlineHint);

  { // set name of newfunc arguments and complete args
    Function::arg_iterator nai = newfunc->arg_begin(), oai = oldfunc->arg_begin();
    for (unsigned i = 0; i!=args_size; ++i, ++oai)
      if (args[i]==NULL) {
        nai->setName(oai->getName());
        args[i] = nai;
        ++nai;
      }
    assert(nai==newfunc->arg_end());
    assert(oai==oldfunc->arg_end());
  }

  // create call to old function
  BasicBlock *BB = BasicBlock::Create(Context, "entry", newfunc);
  IRB->SetInsertPoint(BB);
  ci = IRB->CreateCall(oldfunc,ArrayRef<Value*>(args_begin,args_end));
  if (ci->getType()->isVoidTy())
    IRB->CreateRetVoid();
  else
    IRB->CreateRet(ci);
  if (PrintEach)
    *Out <<"A: " <<*newfunc;

  // the function should be valid now
  verifyFunction(*newfunc);

  // compute the identifying string
  std::string id;
  raw_string_ostream oss(id);
  oss <<*ci <<'\n';
  oss.flush();
  specfun_map::iterator i = spec_functions.find(id.c_str());
  if (i==spec_functions.end()) {
    already = false; // new specialized function
    char *s = new char[id.size()+1];
    strcpy(s,id.c_str());
    spec_functions[s] = newfunc;
  } else {
    already = true; // existing specialized function
    ci = NULL;
    newfunc->eraseFromParent();
    newfunc = i->second;
    assert(newfunc);
  }
}


/**
 * tweto_specialize__optimize
 *
 * Optimize the given function using : loads replacement, 
 * indirect calls resolution, calls specialization
 */
void tweto_specialize__optimize(Function *newfunc, CallInst *ci,
                                ExecutionEngine *EE, Module *Mod, Function *oldfunc,
                                Value **args_begin, Value **args_end) {
  assert(newfunc);
  formatted_raw_ostream *Out = &fouts();
  // LLVMContext &Context = getGlobalContext();
  { // Inline the call
    InlineFunctionInfo i(NULL, TD);
    bool success = InlineFunction(ci, i);
    assert(success);
    verifyFunction(*newfunc);
  }
  // Optimize
  int count;
  do {
    if (PrintEach)
      *Out <<"X: " <<*newfunc;
    FPM->run(*newfunc);
    if (PrintEach)
      *Out <<"B: " <<*newfunc;
    verifyFunction(*newfunc);
    count = 0;
    count += execute_constant_loads(newfunc);
    count += resolve_indirect_calls(newfunc,Mod,EE);
    count += specialize_calls(EE,Mod,newfunc);
  } while (count>0);
  // Check again and return
  if (PrintEach)
    *Out <<"F: " <<*newfunc;
  verifyFunction(*newfunc);
  // std::cerr <<"tweto: function specialized.\n";
}


/**
 * execute_constant_loads
 *
 * Replace 'inttoptr' instructions of the given function
 * by a constant integer (only if the 'inttoptr' argument is a member
 * of const_addresses)
 *
 * inttoptr syntax : <result> = inttoptr <ty> <value> to <ty2>
 * inttoptr exemple :  %X = inttoptr i32 255 to i32*
 */
int execute_constant_loads(Function *F) {
  std::vector<std::pair<LoadInst*,Constant*> > replacements;
  for (inst_iterator ii = inst_begin(F), iie = inst_end(F); ii != iie; ++ii) {
    Instruction &i = *ii;
    if (LoadInst *load_inst = dyn_cast<LoadInst>(&i))
      if (ConstantExpr *ce = dyn_cast<ConstantExpr>(load_inst->getPointerOperand()))
        if (ce->isCast() && !strcmp(ce->getOpcodeName(),"inttoptr"))
          if (ConstantInt *ci = dyn_cast<ConstantInt>(ce->getOperand(0))) {
            const intptr_t ip = ci->getZExtValue();
            if (is_member(const_addresses,ip)) {
              Type *lt = load_inst->getType();
              if (lt==i32) {
                ConstantInt *i32val =
                  ConstantInt::getSigned(i32,*reinterpret_cast<uint32_t*>(ip));
                replacements.push_back(std::pair<LoadInst*,
                                       Constant*>(load_inst,i32val));
              } else if (lt==i64) {
                ConstantInt *i64val =
                  ConstantInt::getSigned(i64,*reinterpret_cast<uint64_t*>(ip));
                replacements.push_back(std::pair<LoadInst*,
                                       Constant*>(load_inst,i64val));
              } else if (lt==i8) {
                ConstantInt *i8val =
                  ConstantInt::getSigned(i8,*reinterpret_cast<uint8_t*>(ip));
                replacements.push_back(std::pair<LoadInst*,
                                       Constant*>(load_inst,i8val));
              } else if (lt==i16) {
                ConstantInt *i16val =
                  ConstantInt::getSigned(i16,*reinterpret_cast<uint16_t*>(ip));
                replacements.push_back(std::pair<LoadInst*,
                                       Constant*>(load_inst,i16val));
              } else if (lt->isPointerTy()) {
                ConstantInt *ipval =
                  ConstantInt::getSigned(i_ptr, *reinterpret_cast<intptr_t*>(ip));
                Constant *pval = ConstantExpr::getIntToPtr(ipval,lt);
                replacements.push_back(std::pair<LoadInst*,Constant*>(load_inst,pval));
              }
            }
          }
  }
  for (size_t i = 0, ei = replacements.size(); i!=ei; ++i) {
    // std::cerr <<"tweto: load inttoptr found -> i32: " <<i32val <<"\n";
    BasicBlock::iterator ii_r(replacements[i].first);
    ReplaceInstWithValue(replacements[i].first->getParent()->getInstList(),
                         ii_r, replacements[i].second);
  }
  if (replacements.size()>0) {
    // std::cerr <<"tweto: execute_constant_loads: "
    //           <<std::dec <<replacements.size() <<".\n";
    verifyFunction(*F);
  }
  TotalConstLoad += replacements.size();
  return replacements.size();
}


/**
 * resolve_indirect_calls
 *
 * Replace indirect call instructions in the given function
 * by direct call using the LLVM GlobalValue table
 *
 * exemple : %1 = load <pointer_to_fun>
 *           %2 = call %1(<function args>)
 *           -> call fun
 */
int resolve_indirect_calls(Function *F, Module *Mod, ExecutionEngine *EE) {
  formatted_raw_ostream *Out = &fouts();
  int count = 0;
  for (inst_iterator ii = inst_begin(F), iie = inst_end(F); ii != iie; ++ii) {
    Instruction &i = *ii;
    CallSite cs(&i); // Treat Call and Invoke instructions
    if (cs.getInstruction())
      if (ConstantExpr *ce = dyn_cast<ConstantExpr>(cs.getCalledValue()))
        if (ce->isCast() && !strcmp(ce->getOpcodeName(),"inttoptr"))
          if (ConstantInt *ci = dyn_cast<ConstantInt>(ce->getOperand(0))) {
            const intptr_t ip = ci->getZExtValue();
            const GlobalValue *gv = EE->getGlobalValueAtAddress((void*)ip);
            if (!gv) {
              *Out <<*F;
              assert(false && "cannot find function");
            }
            Function *fun = Mod->getFunction(gv->getName());
             
              // TEST
              std::cerr <<"FUN: " <<  gv->getName().str() << std::endl;
              
            assert(fun);
            cs.setCalledFunction(fun);
            ++count;
            // We may have to change some type information
            for (unsigned i = 0, ei = cs.arg_size(); i!=ei; ++i)
              if (ConstantExpr *ce = dyn_cast<ConstantExpr>(cs.getArgument(i)))
                if (ce->isCast() && !strcmp(ce->getOpcodeName(),"inttoptr"))
                  if (ConstantInt *ci = dyn_cast<ConstantInt>(ce->getOperand(i))) {
                    Type *t = fun->getFunctionType()->getParamType(i);
                    cs.setArgument(i,ConstantExpr::getIntToPtr(ci,t));
                  }
          }
  }
  if (count>0) {
    // std::cerr <<"tweto: resolve_indirect_calls: " <<std::dec <<count <<".\n";
    verifyFunction(*F);
  }
  return count;
}


/**
 * specialize_calls
 *
 * Replace calls by calls to specialized functions that are 
 * created by replacement of variables in the body code
 * by constants given as argument (to rewrite : not clear) 
 *
 * exemple : read(int a){a+1;} -> read_42(){42+1;}
 */
int specialize_calls(ExecutionEngine *EE, Module *Mod, Function *F) {
  formatted_raw_ostream *Out = &fouts();
  int count = 0;
  bool repeat;
  do {
    repeat = false;
    for (inst_iterator ii = inst_begin(F), iie = inst_end(F); ii != iie; ++ii) {
      Instruction &i = *ii;
      CallSite cs(&i); // Treat Call and Invoke instructions
      if (cs.getInstruction())
        if (Function *oldfun = cs.getCalledFunction())
          if (!oldfun->isDeclaration()) {
            // Candidate for specialization
            const unsigned n = oldfun->getFunctionType()->getNumParams();
            Value **args_spec = new Value*[n];
            Value **args_keep = new Value*[n];
            Value **args_keep_end = args_keep;
            bool can_specialize = false;
            for (unsigned i = 0; i!=n; ++i)
              if (Constant *c = dyn_cast<Constant>(cs.getArgument(i))) {
                args_spec[i] = c;
                can_specialize = true;
              } else {
                args_spec[i] = NULL;
                *args_keep_end = cs.getArgument(i);
                ++args_keep_end;
              }
            if (can_specialize) {
              bool already = false;
              CallInst *ci = NULL;
              Function *newfun = NULL;
              tweto_specialize__create(Mod,oldfun,args_spec,args_spec+n,
                                       already,newfun,ci);
              assert(newfun);
              // const AttrListPtr &attributes = cs.getAttributes();
              if (cs.isInvoke()) {
                InvokeInst *i = dyn_cast<InvokeInst>(cs.getInstruction());
                assert(i);
                BasicBlock *bb1 = i->getNormalDest();
                BasicBlock *bb2 = i->getUnwindDest();
                InvokeInst *newinvoke =
                  InvokeInst::Create(newfun,bb1,bb2,ArrayRef<Value*>(args_keep,args_keep_end),"",i);
                // newinvoke->setAttributes(attributes);
                BasicBlock::iterator ii_r(i);
                ReplaceInstWithValue(i->getParent()->getInstList(),
                                     ii_r, newinvoke);
              } else {
                CallInst *newcall =
                  CallInst::Create(newfun,ArrayRef<Value*>(args_keep,
                                   args_keep_end),"",cs.getInstruction());
                // newcall->setAttributes(attributes);
                BasicBlock::iterator ii_r(cs.getInstruction());
                ReplaceInstWithValue(cs.getInstruction()->getParent()->getInstList(),
                                     ii_r, newcall);
              }
              ++count;
              repeat = true;
              //if (PrintEach) {
              //     *Out <<"AFTER REPLACE\n" <<*F;
              //     verifyFunction(*F);
              //}
              if (!already) {
                assert(ci);
                tweto_specialize__optimize(newfun,ci,
                                           EE,Mod,oldfun,args_spec,args_spec+n);
              }
              //if (PrintEach) {
              //  *Out <<"AFTER OPTIMIZE\n" <<*F;
              //  verifyFunction(*F);
              //}
              break;
            }
            delete[] args_spec;
            delete[] args_keep;
          }
    }
  } while (repeat);
  if (count>0) {
    // std::cerr <<"tweto: specialize_calls      : " <<std::dec <<count <<".\n";
    verifyFunction(*F);
  }
  return count;
}


/**
 * tweto_print_all_specialized
 */
void tweto_print_all_specialized() {
  formatted_raw_ostream *Out = &fouts();
  for (specfun_map::iterator i = spec_functions.begin(), ei = spec_functions.end();
       i!=ei; ++i)
    *Out <<*i->second;
}


/**
 * tweto_print_all_specialized_if_asked
 */
void tweto_print_all_specialized_if_asked() {
  if (PrintAllSpecialized)
    tweto_print_all_specialized();
}
