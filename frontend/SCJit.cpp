#include "SCJit.hpp"

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/DataLayout.h"
//#include "llvm/Analysis/Dominators.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/Verifier.h"
#include "llvm/PassManager.h"
#include "llvm/IR/IRBuilder.h"

#include <sstream>
#include <iostream>

using namespace llvm;

SCJit::SCJit(Module * mod, SCElab * scelab)
{
	this->mdl = mod;
	this->elab = scelab;

	//this->moduleProvider = new ExistingModuleProvider(mod);
	this->ee = EngineBuilder(mod).create();

	//  llvm::ExecutionEngine::create(this->mdl);
	
	if (this->ee == NULL) {
		std::cerr << "Error : executionengine could not be created.\n";
		exit(1);
	}

	this->ee->runStaticConstructorsDestructors(false);
	this->nbFctToJit = 0;
}

SCJit::~SCJit()
{
	this->ee->clearAllGlobalMappings();
 	this->ee->runStaticConstructorsDestructors(true);
 	delete this->ee;
}

void
 SCJit::doFinalization()
{
	;
}

ExecutionEngine*
SCJit::getEngine()
{
	return this->ee;
}


Module *SCJit::getModule()
{
	return this->mdl;
}

SCElab *SCJit::getElab()
{
	return this->elab;
}


void SCJit::elaborate()
{
	/* TODO */
}

void SCJit::setCurrentProcess(Process * process)
{
	this->currentProcess = process;
	if (!process)
		return;
	TRACE_4("#################### SET Process : " << process << "\n");
	TRACE_4("#################### Associated module : " << process->getModule() << "\n");
}

Process *SCJit::getCurrentProcess()
{
	if (!this->currentProcess)
		return NULL;
	TRACE_4("#################### GET current process : " << this->currentProcess << "\n");
	TRACE_4("#################### Associated module : " << this->currentProcess->getModule() << "\n");
	return this->currentProcess;
}

void
pushInst(Instruction * inst, std::vector < Instruction * >&temp_queue,
	 std::vector < Instruction * >&inst_queue, bool temp)
{
	if (temp) {
		if (temp_queue.size() > 0) {
			std::vector < Instruction * >::iterator invalid =
			    remove(temp_queue.begin(), temp_queue.end(),
				   inst);
			temp_queue.erase(invalid, temp_queue.end());
		}
		temp_queue.push_back(inst);
	}

	if (inst_queue.size() > 0) {
		std::vector < Instruction * >::iterator invalid =
		    remove(inst_queue.begin(), inst_queue.end(), inst);
		inst_queue.erase(invalid, inst_queue.end());
	}

	inst_queue.push_back(inst);
}


void buildFctToJit2(Function * fctToJit, Value * res)
{
	BasicBlock *block =
	    BasicBlock::Create(getGlobalContext(), "entry", fctToJit);
	IRBuilder <> builder(block);
	AllocaInst *al =
	    builder.CreateAlloca(IntegerType::get(getGlobalContext(), 32),
				 0, "myvar");
	Constant *ci =
	    ConstantInt::get(IntegerType::get(getGlobalContext(), 32), 42);
	builder.CreateStore(ci, al);
	LoadInst *li = builder.CreateLoad(al);
	builder.CreateRet(li);
}


Value *getValue(std::map < std::string, Value * >namedValues, Value * arg)
{
	if (isa < Constant > (arg)) {
		return arg;
	} else {
		Instruction *inst = cast < Instruction > (arg);
		if (arg->hasName()) {
			return namedValues[arg->getName()];
		} else {
			return inst->clone();
		}
	}
}

Function *SCJit::buildFct(Function * f, FunctionType * FT, Instruction* inst, Value * arg)
{
	Function *fctToJit;

	std::stringstream ss;
	ss << f->getName().str() << "_fctToJit-" << this->nbFctToJit++;
	fctToJit = Function::Create(FT, Function::PrivateLinkage, ss.str(), this->mdl);
	fctToJit->setCallingConv(CallingConv::C);
	TRACE_4("Building fctToJit : " << fctToJit->getName().str() << " " << fctToJit << "\n");

	FunctionBuilder fb(f, fctToJit, inst, arg);
	if (fb.buildFct() == NULL) {
		TRACE_5("[WARNINING] Unable to build Function to Jit\n");
		return NULL;
	}

	TRACE_5("------------ fctToJit completed ---------------\n");
	PRINT_5(fctToJit->dump());
	TRACE_5("-----------------------------------------------\n\n");

	//  fctToJit->viewCFG();

	FunctionPassManager FPM(this->mdl);

	// Set up the optimizer pipeline.  Start with registering info about how the
	// target lays out data structures.
	DataLayout *dl = new DataLayout(*this->ee->getDataLayout());
	DataLayoutPass* dlpass = new DataLayoutPass(*dl);

	FPM.add(dlpass);
	// Promote allocas to registers.
	//   FPM.add(createPromoteMemoryToRegisterPass());
	//    MemoryDependenceAnalysis* mda = new MemoryDependenceAnalysis();
	//    FPM.add(mda);

	//   SelectionDAGISel* sdis =  new SelectionDAGISel(getTargetLowering());
	//   FPM.add(sdis);

	//   DominatorTrthis->ee *d = new DominatorTree();
	//  FPM.add(d);

	FPM.run(*fctToJit);

	verifyFunction(*fctToJit);
	TRACE_5("fctToJit verified\n");

	return fctToJit;
}

void SCJit::fillArgsType(Function * f, std::vector <Type * >*argsType)
{
	// if f is a SC_ENTRY_FUNC_OPT, there is no arg
	// but calling it with a superfluous arg is no problem
	// (in x86 and amd64 cc at least)
	if (f->getArgumentList().empty())
		return;
	Type *t = f->getArgumentList().front().getType();
	argsType->push_back(t);
}

