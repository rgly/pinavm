#include "FunctionBuilder.h"
#include "llvm/ADT/ilist.h"

FunctionBuilder::FunctionBuilder(Function * origFunction,
				Function * functionToJit,
				Value * resValue)
{
	this->origFct = origFunction;
	this->fctToJit = functionToJit;
	this->res = resValue;
}

FunctionBuilder::~FunctionBuilder()
{
//	TRACE_5("Deleting Function Builder...");
//   Utils::deleteVector<BasicBlock*>(this->used_bb);
//   Utils::deleteVector<Instruction*>(this->used_insts);
//	ValueMap.clear();
}

// Looks up the type in the symbol table and returns a pointer to its name or
// a null pointer if it wasn't found. Note that this isn't the same as the
// Mode::getTypeName function which will return an empty string, not a null
// pointer if the name is not found.
inline const std::string
findTypeName(const TypeSymbolTable & ST, const Type * Ty)
{
	TypeSymbolTable::const_iterator TI = ST.begin();
	TypeSymbolTable::const_iterator TE = ST.end();
	for (; TI != TE; ++TI)
		if (TI->second == Ty)
			return TI->first;
	return 0;
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

//     if (isa<PointerType>(arg->getType())) {
//       const PointerType* PTy = cast<const PointerType>(arg->getType());
//       if (isa<StructType>(PTy->getElementType())) {
//      const StructType* STy = cast<const StructType>(PTy->getElementType());
//      const std::string tName = findTypeName(this->mdl->getTypeSymbolTable(), STy);
//       }
//     }
	}


	if (isa < Constant > (arg) || isa < GlobalValue > (arg)
	    || isa < BitCastInst > (arg)) {
		TRACE_6
		    ("** Arg is an arg or constant or global value or bitcast\n");
		return false;
	}

	/* Do not consider some instructions as well as already-visited instructions */
	Instruction *argAsInst = cast < Instruction > (arg);
	if (find(used_insts.begin(), used_insts.end(), argAsInst) != used_insts.end()) {
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
	if (find(used_bb.begin(), used_bb.end(), currentBlock) == used_bb.end()) {
		TRACE_5("Block useful :" << currentBlock->
			getNameStr() << "\n");
		used_bb.push_back(currentBlock);
	}

	return true;
}

void FunctionBuilder::markUsefulInstructions()
{
	Value *arg;
	
	while (!temp_queue.empty()) {
		Instruction *inst = temp_queue.back();
		User::op_iterator opit = inst->op_begin(), opend = inst->op_end();

		temp_queue.pop_back();
		
		/*** Visit each argument of the instruction ***/
		for (; opit != opend; ++opit) {
			arg = *opit;
			
			/*** Mark the instruction and the associated basicblock as useful ***/
			if (!mark(arg))
				continue;
			
			Value::use_iterator I = arg->use_begin(), E = arg->use_end();
			/*** Visit each use of the arg ***/
			for (; I != E; ++I) {
				Value *v = *I;
				
				/*** Mark the instruction and the associated basicblock as useful ***/
				if (!mark(v))
					continue;
			}
		}
	}
}

void FunctionBuilder::cloneBlocks()
{
	for (Function::iterator BI = origFct->begin(), BE = origFct->end(); BI != BE; ++BI) {
		BasicBlock *BB = &*BI;
		
		/*** Avoid basic blocks with no useful instruction ***/
		if (find(used_bb.begin(), used_bb.end(), BB) ==	used_bb.end())
			continue;
		
		/*** Register the (new) basic block in the value map, set its (new) name ***/
		BasicBlock *NewBB = BasicBlock::Create(getGlobalContext(), "", fctToJit);
		if (BB->hasName())
			NewBB->setName(BB->getName());

		TRACE_6("Clone block : " << BB << "->" << BB << " (" <<	NewBB->getNameStr() << ")\n");
		ValueMap[BB] = NewBB;
	}
}

Function *FunctionBuilder::buildFct()
{
	BasicBlock *currentBlock = NULL;

	/********************* Init stack ****************/
	if (isa < Constant > (this->res)) {
		BasicBlock *entryBlock = BasicBlock::Create(getGlobalContext(), "entry", fctToJit);
		Value *instr = IRBuilder <> (entryBlock).CreateRet(res);
		TRACE_5("Function is just a return :\n");
		PRINT_5(instr->dump());
		TRACE_5("\n");
		return this->fctToJit;
	}

	/*********** Determine which instructions are useful ***********/
	TRACE_5("Marking useful basic blocks and instructions\n");
	mark(res);
	markUsefulInstructions();
	
	/**************** Clone blocks *******************/
	TRACE_5("Cloning basic blocks\n");
	cloneBlocks();
	
	/********* Put arguments in the ValueMap *********/
	Function::arg_iterator origIt = this->origFct->arg_begin();
	Function::arg_iterator argIt = this->fctToJit->arg_begin();
	while (origIt != this->origFct->arg_end()) {
		ValueMap[origIt] = argIt;
		origIt++;
		argIt++;
	}
	
	/****** Useful instructions are pushed in the list of *********/
	/************* instructions to be generated  ******************/
	Function::iterator origbb = origFct->begin(), origbe = origFct->end();;
	//  Function::iterator clonebb = cloneFct->begin(), clonebe = cloneFct->end(); ;
	std::map < std::string, Value * >namedValues;
	Instruction *lastInst = NULL;
	BasicBlock *lastBlock = NULL;
	BasicBlock *NewBB;
	BasicBlock *oldCurrentBlock = NULL;
	
	/*** For each basic block in the original function... ***/
	while (origbb != origbe) {
		BasicBlock::iterator origInstIt = origbb->begin(), origInstEnd = origbb->end();
		currentBlock = &*origbb++;
		TRACE_6("Current block : " << currentBlock->getNameStr() << "\n");
		
		/*** ...get the corresponding block in the fctToJit if it exists ***/
		if (find(used_bb.begin(), used_bb.end(), currentBlock) ==
			used_bb.end()) {
			TRACE_6("   not useful\n");
			continue;
		}
		
		NewBB = cast < BasicBlock > (ValueMap[currentBlock]);
		TRACE_6("New block is " << NewBB << "\n");
		
		/*** Add branch instruction from the previous block to the current one ***/
		if (oldCurrentBlock != NULL) {
			oldCurrentBlock->getInstList().push_back(BranchInst::Create(NewBB));
		}
		lastBlock = NewBB;
		
		/*** For each instruction in the original basic block... ***/
		while (origInstIt != origInstEnd) {
			Instruction *origInst = &*origInstIt;

			/*** ...clone the instruction if it is useful ***/
			if (find(used_insts.begin(), used_insts.end(), origInst) != used_insts.end()) {
				Instruction *NewInst = origInst->clone(getGlobalContext());
				TRACE_6("Found useful and cloned : " <<	origInst << " -> " << NewInst << "\n");
				if (origInst->hasName())
					NewInst->setName(origInst->getName());
				//NewBB->getInstList().insert(NewBB->getInstList().end(), NewInst);
				NewBB->getInstList().push_back(NewInst);
				RemapInstruction(NewInst, ValueMap);	// Important: link the instruction to others (use-def chain)
				TRACE_6("Instruction remapped : ");
				PRINT_6(NewInst->dump());
				TRACE_6("\n");
				/* Add instruction to ValueMap so that future calls to RemapInstruction() work. It works because: */
				/* - all variables used have been added in the ValueMap when they have been defined           */
				/* - the basic blocks are already in the ValueMap (necessary for branch instructions)             */
				ValueMap[origInst] = NewInst;
				lastInst = NewInst;
			}
			origInstIt++;
		}
		oldCurrentBlock = NewBB;
	}

	TRACE_6("Number of basic blocks : " << this->fctToJit->getBasicBlockList().size() << "\n");
	TRACE_6("Entry block : " << &this->fctToJit->getEntryBlock() << "\n");
	TRACE_6("Before CreateRet, lastBlock = " << lastBlock << "\n");
	IRBuilder <> (lastBlock).CreateRet(lastInst);
	return this->fctToJit;
}
