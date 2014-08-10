#include "FunctionBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include <llvm/IR/CallSite.h>
#include "llvm/IR/IRBuilder.h"

#include <algorithm>

FunctionBuilder::FunctionBuilder(Function * origFunction,
				Function * functionToJit,
				Instruction* inst,
				Value * resValue)
{
	this->origFct = origFunction;
	this->fctToJit = functionToJit;
	this->res = resValue;
	this->targetInst = inst;
}

FunctionBuilder::~FunctionBuilder()
{
//	TRACE_5("Deleting Function Builder...");
//   Utils::deleteVector<BasicBlock*>(this->used_bb);
//   Utils::deleteVector<Instruction*>(this->used_insts);
//	ValueMap.clear();
}

bool FunctionBuilder::isMarked(Instruction* ins) {
	if (std::find(this->used_insts.begin(), this->used_insts.end(), ins)
			 != this->used_insts.end()) {
		return true;
	} else {
		return false;
	}
}

bool FunctionBuilder::isMarked(BasicBlock* bb) {
	if (std::find(this->used_bb.begin(), this->used_bb.end(), bb)
			!= this->used_bb.end()) {
		return true;
	} else {
		return false;
	}
}

bool FunctionBuilder::mark(Value * arg)
{
 	TRACE_5("Mark() : \n");
 	PRINT_5(arg->dump());
 	TRACE_5("\n");

	if (isa < Argument > (arg)) {
		TRACE_5("Found argument: \n");
		return false;
//     if (argNb == 1) {
//       TRACE_5("Arg is this \n");
//     }

	}


	if (isa < Constant > (arg) || isa < GlobalValue > (arg)) {
		TRACE_6("** Arg is a constant or global value\n");
		return false;
	}

	if (isa < BitCastInst > (arg) &&
	    (!arg->getType()->isPointerTy())) {
		TRACE_6("** Arg is a bitcast other than a pointer\n");
		return false;
	}

	/* Do not consider some instructions as well as already-visited instructions */
	Instruction *argAsInst = cast < Instruction > (arg);
	if (isMarked(argAsInst)) {
		TRACE_6("Arg has already been visited\n");
		return false;
	}

	TRACE_5("Useful : " << argAsInst << "\n");
	PRINT_6(argAsInst->dump());
	TRACE_6("\n");

	used_insts.push_back(argAsInst);
	temp_queue.push_back(argAsInst);

	BasicBlock *currentBlock = argAsInst->getParent();
	if (! isMarked(currentBlock) ) {
		TRACE_5("Block useful :" << currentBlock->getName().str() << "\n");
		this->used_bb.push_back(currentBlock);
	}

	return true;
}

bool
FunctionBuilder::isBeforeTargetInst(Value* v)
{
	return std::find(this->predecessors->begin(), this->predecessors->end(), v) != this->predecessors->end();
}


// FIXME :: RGLY: this function finds the instructions which uses target Inst,
//	and mark them, Then check all the operand of target Inst. If there
//	is a unwanted phinode there, This function don't mark the target Inst.
//	How can this function marking those instructions which use target Inst
//	without marking the target Inst?

bool
FunctionBuilder::markUsefulInstructions()
{
	
	while (!temp_queue.empty()) {
		Instruction *inst = temp_queue.back();
		temp_queue.pop_back();

		TRACE_5("Marking inst " << inst << "\n");
		PRINT_5(inst->dump());
		
		/********************* Visit each use  ***************/
		Value::use_iterator I = inst->use_begin(), E = inst->use_end();
		TRACE_6("> Visiting uses...\n");
		for (; I != E; ++I) {
			Value *v = *I;
			Instruction* vAsInst = dyn_cast<Instruction>(v);
			TRACE_6("Use : " << v << "\n");
			PRINT_5(v->dump());
			/*** Mark the instruction and the associated basicblock as useful ***/
			// MM: TODO: this should use
			// MM: TODO: CallSite::getArgument instead. It's
			// MM: TODO: probably broken.
			if (isBeforeTargetInst(v) && ((isa<StoreInst>(v) && vAsInst->getOperand(1) == inst) ||
							isa<CallInst>(v)))
			{
				mark(v);
			}
		}
		TRACE_6("> Uses visited\n");

	
		/*********** Visit each argument of the instruction ***********/
		TRACE_6("> Visiting args...\n");
		User::op_iterator opit = inst->op_begin(), opend = inst->op_end();
		Value *arg;
		for (; opit != opend; ++opit) {
			arg = *opit;
			TRACE_6("Arg : " << arg << "\n");
			PRINT_6(arg->dump());

			if (isa<PHINode>(arg)) {
				PHINode* pn = dyn_cast<PHINode>(arg);
				int numIncoming = pn->getNumIncomingValues();
				for (int i = 0; i < numIncoming; ++i) {
					BasicBlock* incomingBB = pn->getIncomingBlock(i);
					if (! isBeforeTargetInst(&incomingBB->front()))
						return true;
				}
			}
			/*** Mark the instruction and the associated basicblock as useful ***/
			if (isBeforeTargetInst(arg))
				mark(arg);
		}
		TRACE_5("... marking done for inst : " << inst << "\n");
	}
	return false;
}

void FunctionBuilder::cloneBlocks()
{
	for (Function::iterator BI = origFct->begin(), BE = origFct->end(); BI != BE; ++BI) {
		BasicBlock *BB = &*BI;
		
		/*** Avoid basic blocks with no useful instruction ***/
		if (! isMarked(BB))
			continue;
		
		/*** Register the (new) basic block in the value map, set its (new) name ***/
		BasicBlock *NewBB = BasicBlock::Create(getGlobalContext(), "", fctToJit);
		if (BB->hasName())
			NewBB->setName(BB->getName());

		TRACE_6("Clone block : " << BB << "->" << NewBB << " (" <<	NewBB->getName().str() << ")\n");
		ValueMap[BB] = NewBB;
	}
}

// Collect all the instructions before targetInst to predecessors.
std::vector<Instruction*>* FunctionBuilder::fillPredecessors(Function* origFct,
  Instruction* targetInst) {

	std::vector<Instruction*>* ret = new std::vector<Instruction*>();
	Function::iterator bb = origFct->begin();
	Function::iterator be = origFct->end();

	for (; bb != be; ++bb) {
		BasicBlock::iterator i = bb->begin(), ie = bb->end();
		for (; i != ie; ++i) {
			Instruction* currentInst = &*i;
			if (currentInst != targetInst)
				ret->push_back(currentInst);
			else
				return ret;
		}
	}
	return NULL;
}

void FunctionBuilder::cloneEachInst(BasicBlock* origbb, BasicBlock* NewBB) {
	BasicBlock::iterator origInstIt = origbb->begin(), origInstEnd = origbb->end();
	/*** For each instruction in the original basic block... ***/
	for(; origInstIt != origInstEnd ; ++origInstIt) {
		Instruction *origInst = &*origInstIt;

		/*** ...clone the instruction if it is useful ***/
		if (isMarked(origInst)) {
			Instruction *NewInst = origInst->clone();
			TRACE_6("Found useful and cloned : " <<	origInst << " -> " << NewInst << "  ");
			PRINT_6(origInst->dump());
			if (origInst->hasName())
				NewInst->setName(origInst->getName());

			NewBB->getInstList().push_back(NewInst);
			// Add instruction to ValueMap so that future calls to RemapInstruction() work. It works because:  
			// - all variables used have been added in the ValueMap when they have been defined
			// - the basic blocks are already in the ValueMap (necessary for branch instructions)
			this->ValueMap[origInst] = NewInst;
		}
	}
}

	/********* Put arguments in the ValueMap *********/
void FunctionBuilder::cloneArguments(Function* origFct, Function* fctToJit) {
	Function::arg_iterator origIt = origFct->arg_begin();
	Function::arg_iterator argIt = fctToJit->arg_begin();
	while (origIt != origFct->arg_end()) {
		this->ValueMap[origIt] = argIt;
		++origIt;
		++argIt;
	}
}

/****** Useful instructions are pushed in the list of *********/
/************* instructions to be generated  ******************/
BasicBlock* FunctionBuilder::fillClonedBBWithMarkedInst() {
	BasicBlock *lastBlock = NULL;
	BasicBlock *oldCurrentBlock = NULL;
	Function::iterator origbb = this->origFct->begin();
	Function::iterator origbe = this->origFct->end();

	/*** For each basic block in the original function... ***/
	for(;origbb != origbe ; ++origbb) {
		BasicBlock* currentBlock = &*origbb;
		TRACE_6("Current block : " << currentBlock->getName().str() << "\n");
		
		
		/*** ...get the corresponding block in the fctToJit if it exists ***/
		if (! isMarked(currentBlock)) {
			TRACE_6("   not useful\n");
			continue;
		}
				
		BasicBlock* NewBB = cast < BasicBlock > (this->ValueMap[currentBlock]);
		TRACE_6("New block is " << NewBB << "\n");
		
		/*** Add branch instruction from the previous block to the current one ***/
		// FIXME :: RGLY: no branch inst is marked in previous
		//	procedure, the I guess that following code is under
		//	assumption of BasicBlock is going without conditional
		//	branch. so add branch automatically. It may hurt
		//	the basicblock structure, should clone Terminator
		//	Inst instead of unconditional branch.
		if (oldCurrentBlock != NULL) {
			oldCurrentBlock->getInstList().push_back(BranchInst::Create(NewBB));
			// DIRTY HACK -> the previous instruction puts a
			// ref to a new block, which will
			// not be remapped if we don't add it to
			// the value map
			this->ValueMap[NewBB] = NewBB;
		}

		lastBlock = NewBB;
		oldCurrentBlock = NewBB;
		this->cloneEachInst(origbb, NewBB);
	}
	return lastBlock;
}

// After clone each Instructions from original function to JitFunction,
// the operands is still directing to original instructions. Remap Operands
// to make sure that JitFunction will not use original function.
void FunctionBuilder::remapOperands() {

	// TODO : pour chaque nouveau basic block, pour chaque nouvelle instruction, remapper.
	/*** For each basic block in the original function... ***/
	Function::iterator origbb = this->origFct->begin();
	Function::iterator origbe = this->origFct->end();

	// FIXME :: RGLY: Why don't access FctToJit directly?
	//	the follow BasicBlock operations are the same with
	//	"Foreach Instruction in FctToJit()"
	//	"remapInstruction(I, VMap, RF_None)"
	for(; origbb != origbe; ++origbb) {
		BasicBlock* currentBlock = &*origbb;
		/*** ...get the corresponding block in the fctToJit if it exists ***/
		if (!isMarked(currentBlock)) {
			TRACE_6("   not useful\n");
			continue;
		}			

		BasicBlock* NewBB = cast<BasicBlock>(ValueMap[currentBlock]);	
		BasicBlock::iterator instIt = NewBB->begin();
		BasicBlock::iterator instEnd = NewBB->end();

		/*** For each instruction in the original basic block... ***/
		for(;instIt != instEnd ; ++instIt) {
			Instruction *inst = &*instIt;
			/*** ...remap the instruction if it is useful ***/
			TRACE_6("Attempting to remap : ");
			PRINT_6(inst->dump());
			TRACE_6("\n");
			// Important: link the instruction to others (use-def chain)
			RemapInstruction(inst, this->ValueMap, RF_None);	
		}
	}
}

void FunctionBuilder::CreateReturn(BasicBlock* lastBlock) {
	ASSERT(lastBlock != NULL);
	TRACE_6("lastBlock : " << lastBlock << "\n");

	IRBuilder <> retBuilder(lastBlock);
	bool resIsPointer = isa<PointerType>(this->res->getType());
	bool returnIsPointer = isa<PointerType>(this->fctToJit->getReturnType());
				
	if (resIsPointer && ! returnIsPointer) {
		LoadInst* li = retBuilder.CreateLoad(this->ValueMap[this->res]);
		retBuilder.CreateRet(li);
	} else {
		retBuilder.CreateRet(this->ValueMap[this->res]);
	}
}


Function *FunctionBuilder::buildFct()
{
	TRACE_3("******** Function analysed is :");
	PRINT_3(this->origFct->dump());

	/********************* Init stack ****************/
	if (isa < Constant > (this->res)) {
		TRACE_5("Res : "); PRINT_5(this->res->dump());
		BasicBlock *entryBlock = BasicBlock::Create(getGlobalContext(), "entry", fctToJit);
		Value *instr = IRBuilder<>(entryBlock).CreateRet(res);
		TRACE_5("Function is just a return :\n");
		PRINT_5(instr->dump());
		TRACE_5("\n");
		return this->fctToJit;
	}

	this->predecessors = fillPredecessors(this->origFct, this->targetInst);

	/*********** Determine which instructions are useful ***********/
	TRACE_5("Marking useful basic blocks and instructions\n");
	mark(res);
	PRINT_5(res->dump());
	if (markUsefulInstructions())
		return NULL;
	
	TRACE_5("Cloning basic blocks\n");
	this->cloneBlocks();
	this->cloneArguments(origFct, fctToJit);

	// FIXME :: RGLY : I think the last block should be the same
	//		with TargetInst->getParent().
	//		but here returns the physical last BasicBlock here.
	//		(The las BasicBlock of Function::iterator without
	//		 beyond the Target Inst)
	BasicBlock *lastBlock = this->fillClonedBBWithMarkedInst();
	
	this->remapOperands();
	this->CreateReturn(lastBlock);


	TRACE_3("******** [end] Function analysed is :");
	PRINT_3(this->origFct->dump());

	
	TRACE_6("Number of basic blocks : " << this->fctToJit->getBasicBlockList().size() << "\n");
	TRACE_6("Entry block : " << &this->fctToJit->getEntryBlock() << "\n");
	TRACE_6("Before CreateRet, lastBlock = " << lastBlock << "\n");
	delete this->predecessors;
	return this->fctToJit;
}
