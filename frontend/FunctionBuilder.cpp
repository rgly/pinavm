#include "FunctionBuilder.h"
#include "llvm/LLVMContext.h"
#include <llvm/Support/CallSite.h>
#include "llvm/IRBuilder.h"

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

bool FunctionBuilder::mark(Value * arg)
{
	BasicBlock *currentBlock;

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
	if (std::find(used_insts.begin(), used_insts.end(), argAsInst) != used_insts.end()) {
		TRACE_6("Arg has already been visited\n");
		return false;
	} else {
		TRACE_5("Useful : " << argAsInst << "\n");
		PRINT_6(argAsInst->dump());
		TRACE_6("\n");

		used_insts.push_back(argAsInst);
		temp_queue.push_back(argAsInst);
	}

	currentBlock = argAsInst->getParent();
	if (std::find(used_bb.begin(), used_bb.end(), currentBlock) == used_bb.end()) {
		TRACE_5("Block useful :" << currentBlock->getName().str() << "\n");
		used_bb.push_back(currentBlock);
	}

	return true;
}

bool
FunctionBuilder::isBeforeTargetInst(Value* v)
{
	return std::find(this->predecessors->begin(), this->predecessors->end(), v) != this->predecessors->end();
}


int
FunctionBuilder::markUsefulInstructions()
{
	Value *arg;
	
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
						return 1;
				}
			}
			/*** Mark the instruction and the associated basicblock as useful ***/
			if (isBeforeTargetInst(arg))
				mark(arg);
		}
		TRACE_5("... marking done for inst : " << inst << "\n");
	}
	return 0;
}

void FunctionBuilder::cloneBlocks()
{
	for (Function::iterator BI = origFct->begin(), BE = origFct->end(); BI != BE; ++BI) {
		BasicBlock *BB = &*BI;
		
		/*** Avoid basic blocks with no useful instruction ***/
		if (std::find(used_bb.begin(), used_bb.end(), BB) ==	used_bb.end())
			continue;
		
		/*** Register the (new) basic block in the value map, set its (new) name ***/
		BasicBlock *NewBB = BasicBlock::Create(getGlobalContext(), "", fctToJit);
		if (BB->hasName())
			NewBB->setName(BB->getName());

		TRACE_6("Clone block : " << BB << "->" << NewBB << " (" <<	NewBB->getName().str() << ")\n");
		ValueMap[BB] = NewBB;
	}
}

Function *FunctionBuilder::buildFct()
{
	BasicBlock *currentBlock = NULL;

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
	bool end = false;
	this->predecessors = new std::vector<Instruction*>();
	for (Function::iterator bb = this->origFct->begin(), be = this->origFct->end(); bb != be && !end; ++bb) {
		BasicBlock::iterator i = bb->begin(), ie = bb->end();
		while (i != ie && ! end) {
			Instruction* currentInst = &*i;
			if (currentInst == this->targetInst)
				end = true;
			else
				this->predecessors->push_back(currentInst);
			++i;
		}
	}

	/*********** Determine which instructions are useful ***********/
	TRACE_5("Marking useful basic blocks and instructions\n");
	mark(res);
	PRINT_5(res->dump());
	if (markUsefulInstructions())
		return NULL;
	
	/**************** Clone blocks *******************/
	TRACE_5("Cloning basic blocks\n");
	cloneBlocks();
	
	/********* Put arguments in the ValueMap *********/
	Function::arg_iterator origIt = this->origFct->arg_begin();
	Function::arg_iterator argIt = this->fctToJit->arg_begin();
	while (origIt != this->origFct->arg_end()) {
		ValueMap[origIt] = argIt;
		++origIt;
		++argIt;
	}
	
	/****** Useful instructions are pushed in the list of *********/
	/************* instructions to be generated  ******************/
	Function::iterator origbb = origFct->begin(), origbe = origFct->end();;
	//  Function::iterator clonebb = cloneFct->begin(), clonebe = cloneFct->end(); ;
	std::map < std::string, Value * >namedValues;
	BasicBlock *lastBlock = NULL;
	BasicBlock *NewBB;
	BasicBlock *oldCurrentBlock = NULL;
	
	/*** For each basic block in the original function... ***/
	while (origbb != origbe) {
			
		BasicBlock::iterator origInstIt = origbb->begin(), origInstEnd = origbb->end();
				

		currentBlock = &*origbb++;
			
		TRACE_6("Current block : " << currentBlock->getName().str() << "\n");
		
		
		/*** ...get the corresponding block in the fctToJit if it exists ***/
		if (find(used_bb.begin(), used_bb.end(), currentBlock) == used_bb.end()) {
			TRACE_6("   not useful\n");
			continue;
		}
				
		
		NewBB = cast < BasicBlock > (ValueMap[currentBlock]);
			
	
		TRACE_6("New block is " << NewBB << "\n");
		
		/*** Add branch instruction from the previous block to the current one ***/
		if (oldCurrentBlock != NULL) {
			oldCurrentBlock->getInstList().push_back(BranchInst::Create(NewBB));
			ValueMap[NewBB] = NewBB; // DIRTY HACK -> the previous instruction puts a
						 // ref to a new block, which will
						 // not be remapped if we don't add it to
						 // the value map
		}
		lastBlock = NewBB;
		
		/*** For each instruction in the original basic block... ***/
		while (origInstIt != origInstEnd) {
			Instruction *origInst = &*origInstIt;

			/*** ...clone the instruction if it is useful ***/
			if (std::find(used_insts.begin(), used_insts.end(), origInst) != used_insts.end()) {
				Instruction *NewInst = origInst->clone();
				TRACE_6("Found useful and cloned : " <<	origInst << " -> " << NewInst << "  ");
				PRINT_6(origInst->dump());
				if (origInst->hasName())
					NewInst->setName(origInst->getName());
				//NewBB->getInstList().insert(NewBB->getInstList().end(), NewInst);
				NewBB->getInstList().push_back(NewInst);
				/* Add instruction to ValueMap so that future calls to RemapInstruction() work. It works because: */
				/* - all variables used have been added in the ValueMap when they have been defined           */
				/* - the basic blocks are already in the ValueMap (necessary for branch instructions)             */
				ValueMap[origInst] = NewInst;
			}
			++origInstIt;
		}
		oldCurrentBlock = NewBB;
	}

	// TODO : pour chaque nouveau basic block, pour chaque nouvelle instruction, remapper.
	/*** For each basic block in the original function... ***/
	origbb = origFct->begin(), origbe = origFct->end();
	while (origbb != origbe) {
		currentBlock = &*origbb++;
		/*** ...get the corresponding block in the fctToJit if it exists ***/
		if (std::find(used_bb.begin(), used_bb.end(), currentBlock) == used_bb.end()) {
			TRACE_6("   not useful\n");
			continue;
		}			
		NewBB = cast < BasicBlock > (ValueMap[currentBlock]);	
		BasicBlock::iterator instIt = NewBB->begin(), instEnd = NewBB->end();

		/*** For each instruction in the original basic block... ***/
		while (instIt != instEnd) {
			Instruction *inst = &*instIt;
			/*** ...remap the instruction if it is useful ***/
			TRACE_6("Attempting to remap : ");
			PRINT_6(inst->dump());
			TRACE_6("\n");
			
			RemapInstruction(inst, ValueMap, RF_None);	// Important: link the instruction to others (use-def chain)
			++instIt;
		}
	}
	
	ASSERT(lastBlock != NULL);
	TRACE_6("lastBlock : " << lastBlock << "\n");

	IRBuilder <> retBuilder(lastBlock);
				

	bool resIsPointer = isa<PointerType>(this->res->getType());
				

	bool returnIsPointer = isa<PointerType>(this->fctToJit->getReturnType());
				

	if (resIsPointer && ! returnIsPointer) {
					

		LoadInst* li = retBuilder.CreateLoad(ValueMap[this->res]);
		retBuilder.CreateRet(li);
	} else {
			;	

		retBuilder.CreateRet(ValueMap[this->res]);
	}

	TRACE_3("******** [end] Function analysed is :");
	PRINT_3(this->origFct->dump());

	
	TRACE_6("Number of basic blocks : " << this->fctToJit->getBasicBlockList().size() << "\n");
	TRACE_6("Entry block : " << &this->fctToJit->getEntryBlock() << "\n");
	TRACE_6("Before CreateRet, lastBlock = " << lastBlock << "\n");
	delete this->predecessors;
	return this->fctToJit;
}
