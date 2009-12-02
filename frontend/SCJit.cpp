#include "SCJit.hpp"

using namespace std;
using namespace llvm;

SCJit::SCJit(Module * mod, SCElab * scelab)
{
	this->mdl = mod;
	this->elab = scelab;

	this->moduleProvider = new ExistingModuleProvider(mod);
	this->ee = EngineBuilder(this->moduleProvider).create();

	//  llvm::ExecutionEngine::create(this->mdl);

	if (this->ee == NULL) {
		std::
		    cerr <<
		    "Error : executionengine could not be created.\n";
		exit(1);
	}

	this->ee->runStaticConstructorsDestructors(false);
}

SCJit::~SCJit()
{
	this->ee->runStaticConstructorsDestructors(true);
	delete this->ee;
}

void
 SCJit::doFinalization()
{
	;
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
}

Process *SCJit::getCurrentProcess()
{
	return this->currentProcess;
}

void
pushInst(Instruction * inst, vector < Instruction * >&temp_queue,
	 vector < Instruction * >&inst_queue, bool temp)
{
	if (temp) {
		if (temp_queue.size() > 0) {
			vector < Instruction * >::iterator invalid =
			    remove(temp_queue.begin(), temp_queue.end(),
				   inst);
			temp_queue.erase(invalid, temp_queue.end());
		}
		temp_queue.push_back(inst);
	}

	if (inst_queue.size() > 0) {
		vector < Instruction * >::iterator invalid =
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
			return inst->clone(getGlobalContext());
		}
	}
}

Function *SCJit::buildFct(Function * f, FunctionType * FT, Value * arg)
{
	Function *fctToJit;

	fctToJit =
	    Function::Create(FT, Function::ExternalLinkage, "fctToJit",
			     this->mdl);
	fctToJit->setCallingConv(CallingConv::C);

	FunctionBuilder fb(currentProcess, f, fctToJit, arg);
	fb.buildFct();

	TRACE_5("------------ fctToJit completed ---------------\n");
	PRINT_5(fctToJit->dump());
	TRACE_5("-----------------------------------------------\n\n");

	//  fctToJit->viewCFG();

	FunctionPassManager FPM(this->moduleProvider);

	// Set up the optimizer pipeline.  Start with registering info about how the
	// target lays out data structures.
	TargetData *td = new TargetData(*this->ee->getTargetData());

	FPM.add(td);
	// Promote allocas to registers.
	//   FPM.add(createPromoteMemoryToRegisterPass());
	//    MemoryDependenceAnalysis* mda = new MemoryDependenceAnalysis();
	//    FPM.add(mda);

	//   SelectionDAGISel* sdis =  new SelectionDAGISel(getTargetLowering());
	//   FPM.add(sdis);

	//   DominatorTrthis->ee *d = new DominatorTree();
	//  FPM.add(d);

	FPM.run(*fctToJit);
	//  MP.releaseModule();

	verifyFunction(*fctToJit);
	TRACE_5("fctToJit verified\n");

	return fctToJit;
}

void fillArgsType(Function * f, std::vector < const Type * >*argsType)
{
	const Type *t = f->getArgumentList().front().getType();
	argsType->push_back(t);
}

void *SCJit::jitAddr(Function * f, Value * arg)
{
	const std::vector < const Type *>argsType;
	Function *fctToJit;

	TRACE_5("jitAddr()\n");

	fillArgsType(f, (std::vector < const Type * >*) &argsType);
	FunctionType *FT =
	    FunctionType::get(arg->getType(), argsType, false);

	fctToJit = buildFct(f, FT, arg);

	void *(*fct) (sc_core::sc_module *) =
	    (void *(*)(sc_core::sc_module *)) ee->
	    getPointerToFunction(fctToJit);
	TRACE_5("Executing fctToJit\n");
	void *res =
	    fct(this->elab->
		getSCModule(this->currentProcess->getModule()));

	return res;
}


int SCJit::jitInt(Function * f, Value * arg)
{
	Function *fctToJit;
	const std::vector < const Type *>argsType;

	TRACE_5("jitInt() \n");

	fillArgsType(f, (std::vector < const Type * >*) &argsType);
	FunctionType *FT =
	    FunctionType::get(IntegerType::get(getGlobalContext(), 32),
			      argsType, false);

	fctToJit = buildFct(f, FT, arg);

	int (*fct) (sc_core::sc_module *) =
	    (int (*)(sc_core::sc_module *)) ee->
	    getPointerToFunction(fctToJit);
	int res =
	    fct(this->elab->
		getSCModule(this->currentProcess->getModule()));
	delete fctToJit;

	return res;
}

double SCJit::jitDouble(Function * f, Value * arg)
{
	Function *fctToJit;
	const std::vector < const Type *>argsType;

	TRACE_5("jitDouble() \n");
	fillArgsType(f, (std::vector < const Type * >*) &argsType);
	FunctionType *FT =
	    FunctionType::get(Type::getDoubleTy(getGlobalContext()),
			      argsType, false);

	fctToJit = buildFct(f, FT, arg);

	double (*fct) (sc_core::sc_module *) =
	    (double (*)(sc_core::sc_module *)) ee->
	    getPointerToFunction(fctToJit);
	double res =
	    fct(this->elab->
		getSCModule(this->currentProcess->getModule()));

	return res;
}
