#include "SCJit.hpp"

using namespace std;
using namespace llvm;

SCJit::SCJit(Module* mod)
{
  this->mdl = mod;
  
  cout << "creating llvm::ExecutionEngine... ";
  this->ee = llvm::ExecutionEngine::create(this->mdl);
  
  if (ee == NULL) {
    cout << "Error : executionengine could not be created.\n";
    exit(1);
  }
  cout << "done.\n";

  ee->runStaticConstructorsDestructors(false); 
}

void
SCJit::doFinalization()
{
  this->ee->runStaticConstructorsDestructors(true); 
}

Module*
SCJit::getModule() {
  return this->mdl;
}

void
SCJit::elaborate()
{
  /* TODO */
}

void 
pushInst(Instruction* inst, vector<Instruction*>& temp_queue, vector<Instruction*>& inst_queue, bool temp)
{
  if (temp) {
    if (temp_queue.size() > 0) {
      vector<Instruction*>::iterator invalid = remove(temp_queue.begin(), temp_queue.end(), inst);
      temp_queue.erase(invalid, temp_queue.end());
    }
    temp_queue.push_back(inst);
  }
  
  if (inst_queue.size() > 0) {
    vector<Instruction*>::iterator invalid = remove(inst_queue.begin(), inst_queue.end(), inst);
    inst_queue.erase(invalid, inst_queue.end());
  }
  
  inst_queue.push_back(inst);
}


void
buildFctToJit2(Function* fctToJit, Value* res)
{
  BasicBlock *block = BasicBlock::Create(getGlobalContext(), "entry", fctToJit);
  IRBuilder<> builder(block);
  AllocaInst * al = builder.CreateAlloca(IntegerType::get(getGlobalContext(), 32), 0, "myvar");
  Constant* ci = ConstantInt::get(IntegerType::get(getGlobalContext(), 32), 42);
  builder.CreateStore(ci, al);
  LoadInst* li = builder.CreateLoad(al);
  builder.CreateRet(li); 
}


Value*
getValue(std::map<std::string, Value*> namedValues, Value* arg)
{
  if (isa<Constant>(arg)) {
    return arg;
  } else {
    Instruction* inst = cast<Instruction>(arg);
    if(arg->hasName()) {
      return namedValues[arg->getName()];
    } else {
      return inst->clone(getGlobalContext());
    }
  }
}


void
SCJit::buildFctToJit(Function* origFct, Function* fctToJit, Value* res)
{
  vector<Instruction*> temp_queue;
  vector<Instruction*> used_insts;
  vector<BasicBlock*> used_bb;

  Value* instr;
  Value* arg;
  BasicBlock* block = &origFct->getEntryBlock();
  IRBuilder<>* builder;
  BasicBlock* currentBlock = NULL;

  /********************* Init stack ****************/
  if (isa<ConstantInt>(res)) {
    builder = new IRBuilder<>(block);
    instr = builder->CreateRet(res);
    return;
  } else {
    temp_queue.push_back(cast<Instruction>(res));
  }
  
  /*********** Determine which instructions are useful ***********/
  while (! temp_queue.empty()) {
    Instruction* inst = temp_queue.back();
    temp_queue.pop_back();

    /*** Visit each argument of the instruction ***/
    for (User::op_iterator opit = inst->op_begin(), opend = inst->op_end(); opit != opend; ++opit) {
      arg = *opit;

      if (isa<Constant>(arg) || isa<Argument>(arg) || isa<GlobalValue>(arg))
	continue;

      /*** Mark the instruction and the associated basicblock as useful ***/
      Instruction* argAsInst = cast<Instruction>(arg);
      used_insts.push_back(argAsInst);
      currentBlock = argAsInst->getParent();
      if (find(used_bb.begin(), used_bb.end(), currentBlock) == used_bb.end())
	used_bb.push_back(currentBlock);

      /*** Visit each use of the arg ***/
      for (Value::use_iterator I = arg->use_begin(), E = arg->use_end(); I != E; ++I) {
	Value* v = *I;
	Instruction* iuse = cast<Instruction>(v);
	
	/* Do not consider some instructions as well as already-visited instructions*/
	if (isa<BitCastInst>(iuse) || find(used_insts.begin(), used_insts.end(), iuse) != used_insts.end())
	  continue;
	
	/*** Mark the instruction and the associated basic block as useful ***/
	used_insts.push_back(iuse);
	currentBlock = iuse->getParent();
	if (find(used_bb.begin(), used_bb.end(), currentBlock) == used_bb.end())
	  used_bb.push_back(currentBlock);
      }
    }
  }

  cout << "#### Useful basic blocks and instructions marked\n";

  /**************** Clone blocks *******************/
  DenseMap<const Value*, Value*> ValueMap;
  for (Function::iterator BI = origFct->begin(), BE = origFct->end(); BI != BE; ++BI) {
    BasicBlock* BB = &*BI;
    
    /*** Avoid basic blocks with no useful instruction ***/
    if (find(used_bb.begin(), used_bb.end(), BB) == used_bb.end())
      continue;

    /*** Register the (new) basic block in the value map, set its (new) name ***/
    BasicBlock *NewBB = BasicBlock::Create(getGlobalContext(), "", fctToJit);
    if (BB->hasName())
      NewBB->setName(BB->getName());
    
    cout << "Cloned: " << NewBB->getNameStr() << "\n";
    ValueMap[BB] = NewBB;
  }

  cout << "#### Basic blocks cloned\n";
  
  /****** Useful instructions are pushed in the list of *********/
  /************* instructions to be generated  ******************/
  Function::iterator origbb = origFct->begin(), origbe = origFct->end(); ;
  //  Function::iterator clonebb = cloneFct->begin(), clonebe = cloneFct->end(); ;
  std::map<std::string, Value*> namedValues;
  Instruction* lastInst;
  BasicBlock* lastBlock;
  BasicBlock* NewBB;
  BasicBlock* oldCurrentBlock = NULL;

  /*** For each basic block in the original function... ***/
  while(origbb != origbe) {
    BasicBlock::iterator origInstIt = origbb->begin(), origInstEnd = origbb->end();
    currentBlock = &*origbb++;
    cout << "CURRENTBLOCK: "<< currentBlock->getNameStr() << "\n";

    /*** ...get the corresponding block in the fctToJit if it exists ***/
    if (find(used_bb.begin(), used_bb.end(), currentBlock) == used_bb.end()) {
      cout << "   NOT USEFUL\n";
      continue;
    } else {
      NewBB = cast<BasicBlock>(ValueMap[currentBlock]);
    }

    /*** Add branch instruction from the previous block to the current one ***/
    if (oldCurrentBlock != NULL) {
      oldCurrentBlock->getInstList().push_back(BranchInst::Create(NewBB));
    }
    lastBlock = NewBB;

    /*** For each instruction in the original basic block... ***/
    while(origInstIt != origInstEnd) {
      cout << "  start inst loop\n";
      Instruction* origInst = &*origInstIt;
      cout << "  origInst got\n";

      /*** ...clone the instruction if it is useful ***/
      if (find(used_insts.begin(), used_insts.end(), origInst) != used_insts.end()) {
	cout << "  found useful\n";
	Instruction *NewInst = origInst->clone(getGlobalContext());
	if (origInst->hasName())
	  NewInst->setName(origInst->getName());
	RemapInstruction(NewInst, ValueMap);         // Important: link the instruction to others (use-def chain)
	cout << "Instruction remapped\n";
	NewInst->dump();
	NewBB->getInstList().push_back(NewInst);
	/* Add instruction to ValueMap so that future calls to RemapInstruction() work. It works because: */
	/* - all variables used are have been added in the ValueMap when thay have been defined           */
	/* - the basic blocks are already in the ValueMap (necessary for branch instructions)             */
	ValueMap[origInst] = NewInst;
	lastInst = NewInst;	
      }
      cout << "  end of inst loop\n";
      origInstIt++;
    }
    cout << "  end of bb loop\n";
    oldCurrentBlock = NewBB;
  }
  cout << "Before CreateRet\n";
  IRBuilder<>(lastBlock).CreateRet(lastInst);

//   while(origbb != origbe) {
//     BasicBlock::iterator origInstIt = origbb->begin(), origInstEnd = origbb->end();
//     currentBlock = &*clonebb;
//     cout << "CURRENTBLOCK: "<< currentBlock->getNameStr() << "\n";
//     while (! currentBlock->empty()) {
//       (&(currentBlock->front()))->removeFromParent();
//     }
//     currentBlock->getInstList().erase(currentBlock->getInstList().begin(), currentBlock->getInstList().end());

// //     while (! block->empty()) {
// //       cout << "before erase\n";
// //       (&(block->front()))->eraseFromParent();
// //       cout << "after erase\n";
// //     }

//     builder = new IRBuilder<>(currentBlock);

//     while(origInstIt != origInstEnd) {
//       Instruction* origInst = &*origInstIt;
      
//       if (find(used_insts.begin(), used_insts.end(), origInst) != used_insts.end()) {
// 	lastValidBuilder = builder;
// 	if (isa<AllocaInst>(origInst)) {
// 	  cout << "ALLOCA\n";	  	  
// 	  AllocaInst* origAlloc = cast<AllocaInst>(origInst);
// 	  string name = origAlloc->getNameStr();
// 	  AllocaInst* alloc = builder->CreateAlloca(IntegerType::get(32), NULL, name.c_str());
// 	  namedValues[name] = alloc;
// 	  lastInst = alloc;
// 	} else if (isa<LoadInst>(origInst)) {
// 	  cout << "LOAD\n";  
// 	  Value* op1 = getValue(namedValues, origInst->getOperand(0));
// 	  Value* op2 = getValue(namedValues, origInst->getOperand(1));
// 	  cout << "  op1: \n";
// 	  op1->dump();
// 	  cout << "  op2: \n";
// 	  op2->dump();
// 	  lastInst = builder->CreateLoad(op1, op2);
// 	  cout << "LOAD DONE\n";  
// 	} else if(isa<StoreInst>(origInst)) {
// 	  cout << "STORE\n";	  
// 	  Value* op1 = getValue(namedValues, origInst->getOperand(0));
// 	  Value* op2 = getValue(namedValues, origInst->getOperand(1));
// 	  cout << "  op1: \n";
// 	  op1->dump();
// 	  cout << "  op2: \n";
// 	  op2->dump();
// 	  lastInst = builder->CreateStore(op1, op2);
// 	  cout << "STORE DONE\n";	  	  
// 	}//  else {
// // 	  for (User::op_iterator opit = origInst->op_begin(), opend = origInst->op_end(); opit != opend; ++opit) {
// // 	    arg = *opit;
	    
// // 	  }
// // 	}
//       }
//       origInstIt++;
//     }
//     origbb++;
//     clonebb++;
//     cout << "AFTER SIZE: " << currentBlock->getInstList().size() << "\n";

//     if (currentBlock->getInstList().size() == 0) {
//       currentBlock->removeFromParent();
//       cout << "BLOCK REMOVED\n" ;
//       //currentBlock->eraseFromParent();
//     } else {
//       cout << "BLOCK TREATED\n" ;
//       currentBlock->dump();
//     }
//   }
//   cout << "Before ret instruction\n" ;
//   lastValidBuilder->CreateRet(lastInst);
  
}


int
SCJit::jitInt(Function* f, Value* arg)
{
  std::vector<llvm::GenericValue> args;
  Function* fctToJit;
  GenericValue gvint;
  const std::vector<const Type*> argsType;

  cout << "jitInt !\n";

  /* TODO : delete args */
  /* TODO : support more-than-one-call to that function : second time, "fctToJit" will be found !! */
//   c = this->mdl->getOrInsertFunction("fctToJit",
// 				     IntegerType::get(32), /*ret type*/
// 				     NULL);         /*varargs terminated with null*/

  FunctionType* FT = FunctionType::get(IntegerType::get(getGlobalContext(), 32), argsType, false);
  cout << "TY created\n";

  fctToJit = Function::Create(FT, Function::ExternalLinkage, "fctToJit", this->mdl);
  fctToJit->setCallingConv(CallingConv::C);

  //  fctToJit = cast<Function>(c);
  cout << "function inserted : " << fctToJit->getNameStr() << "\n";

  //  arg->dump();
  buildFctToJit(f, fctToJit, arg);
  cout << "fctToJit built !\n";
 
  //  args.push_back(PUSH_BACK() ARGS HERE);
  //  fctToJit->viewCFG();

   ExistingModuleProvider MP(fctToJit->getParent());
   FunctionPassManager FPM(&MP);
   //   FPM.run(*fctToJit);
   //   MP.releaseModule();


   // Set up the optimizer pipeline.  Start with registering info about how the
   // target lays out data structures.
   TargetData* td = new TargetData(*this->ee->getTargetData());
   cout << td->getStringRepresentation();
   FPM.add(td);
   // Promote allocas to registers.
   //   FPM.add(createPromoteMemoryToRegisterPass());
//    MemoryDependenceAnalysis* mda = new MemoryDependenceAnalysis();
//    FPM.add(mda);
   
   //   SelectionDAGISel* sdis =  new SelectionDAGISel(getTargetLowering());
   //   FPM.add(sdis);

   //   DominatorTree *d = new DominatorTree();
   //  FPM.add(d);
  
  fctToJit->dump();

  FPM.run(*fctToJit);
  
  fctToJit->dump();
  verifyFunction(*fctToJit);
  //  gvint = ee->runFunction(fctToJit, args);
  int (*fct)() = (int (*)()) ee->getPointerToFunction(fctToJit);
  int res = fct();
  cout << "fctToJit executed !\n";
   
  return res;
}


// void
// savebuildFctToJit(Function* fctToJit, Value* res)
// {
//   vector<Instruction*> temp_queue;
//   vector<Instruction*> inst_queue;

//   Value* instr;
//   Value* arg;
//   fctToJit->setCallingConv(CallingConv::C);
//   BasicBlock* block = BasicBlock::Create("entry", fctToJit);
//   IRBuilder<> builder(block);

//   if (isa<ConstantInt>(res)) {
//     instr = builder.CreateRet(res);
//     return;
//   } else {
//     temp_queue.push_back(cast<Instruction>(res));
//   }
    
//   while (! temp_queue.empty()) {
//     cout << "-- begin loop\n";
//     Instruction* inst = temp_queue.back();
//     cout << "-- inst get\n";
//     temp_queue.pop_back();

//     pushInst(inst, temp_queue, inst_queue, false);
//     cout << "************** INST *****************\n";
//     inst->dump();
//     cout << "*************************************\n";

//     for (User::op_iterator opit = inst->op_begin(), opend = inst->op_end(); opit != opend; ++opit) {
//       arg = *opit;

//       if (isa<Constant>(arg) || isa<Argument>(arg) || isa<GlobalValue>(arg))
// 	continue;

//       cout << "********** ARG " << arg << " " << opit << " ***************\n";
//       arg->dump();
//       cout << "******************************\n";

//       for (Value::use_iterator I = arg->use_begin(), E = arg->use_end(); I != E; ++I) {
// 	Value* v = *I;
// 	Instruction* iuse = cast<Instruction>(v);

// 	cout << "------ USE " << iuse << " ------\n";
// 	iuse->dump();
// 	cout << "-----------------\n";
	
// 	if (isa<BitCastInst>(iuse) || (int) iuse >= (int) inst)
// 	  continue;
// 	//	if (isa<StoreInst>(iuse) && iuse->getOperand(1) == arg) {
	
// 	pushInst(iuse, temp_queue, inst_queue, true);
// 	cout << "---> Add it: " << iuse << "\n";
// 	  //	}
//       }
//       pushInst(cast<Instruction>(arg), temp_queue, inst_queue, true);
//     }
//   }
//   cout << "-- Producing instructions : " << inst_queue.size() << "\n";
//   Instruction* lastInst;
//   while (! inst_queue.empty()) {
//     cout << "-- BEGIN inst producing loop \n";    
//     Instruction* inst = inst_queue.back();
//     cout << "get\n";
//     inst_queue.pop_back();
//     cout << "popped\n";
//     lastInst = inst->clone();
//     lastInst->setName(inst->getName());
//     inst->dump();
//     cout << "?????????? " << inst->getNameStr() << "\n";
//     lastInst->dump();
//     builder.Insert(lastInst);
//     cout << "inserted\n";
//   }
//   builder.CreateRet(lastInst);
// }
