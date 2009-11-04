#include <systemc.h>

#include "llvm/Support/ManagedStatic.h"
#include "llvm/PassManager.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/StandardPasses.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetSelect.h"

#include "llvm/Support/SystemUtils.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/PassNameParser.h"
#include "llvm/Bitcode/ReaderWriter.h"

#include "llvm/System/Signals.h"

#include "llvm/Pass.h"
#include "llvm/Function.h"
#include "llvm/Module.h"
#include "llvm/CallingConv.h"
#include "llvm/DerivedTypes.h"
#include "llvm/InstrTypes.h"
#include "llvm/Instructions.h"

#include "SCJit.hpp"
#include "SCCFactory.hpp"

using namespace llvm;

extern int launch_systemc(int argc, char *argv[]);


static cl::opt < std::string >
InputFilename(cl::Positional, cl::desc("<input bitcode file>"),
	      cl::init("-"), cl::value_desc("filename"));

static cl::opt < std::string >
OutputFilename("o", cl::desc("Override output filename"),
	       cl::value_desc("filename"), cl::init("-"));

struct FunctionNames:public ModulePass {
	SCJit *scjit;
	SCCFactory *sccfactory;
	Function *mainFct;

      public:
	static char ID;		// Pass identification, replacement for typeid
	FunctionNames():ModulePass(&ID) {
	} bool runOnModule(Module & M) {
		Module *mod = &M;
		Function *F;


		llvm::cout << "Pinapa --> do initialization\n";
		llvm::cout << "\n";

		llvm::cout << "Pinapa --> pinapa_callback\n";

		// To call the "end_of_elaboration()" methods.
		sc_get_curr_simcontext()->initialize(true);

		this->scjit = new SCJit(mod);
		this->sccfactory = new SCCFactory(scjit);
		this->mainFct = mod->getFunction("main");

		std::vector < Function * >*fctStack =
		    new std::vector < Function * >();
		fctStack->push_back(this->mainFct);

		while (!fctStack->empty()) {
			F = fctStack->back();
			fctStack->pop_back();

			for (Function::iterator bb = F->begin(), be =
			     F->end(); bb != be; ++bb) {
				BasicBlock::iterator i = bb->begin(), ie =
				    bb->end();
				while (i != ie) {
					CallInst *callInst =
					    dyn_cast < CallInst > (&*i);
					if (callInst) {
						if (!sccfactory->
						    handle(F, &*bb,
							   callInst)) {
							fctStack->
							    push_back
							    (callInst->
							     getCalledFunction
							     ());
						}
					}
					BasicBlock::iterator tmpend =
					    bb->end();
					i++;
				}
			}
		}

		this->scjit->doFinalization();
		return false;
	}

	void getAnalysisUsage(AnalysisUsage & AU) const {
		AU.setPreservesAll();
}};

void pinapa_callback()
{
	llvm::cout << "Entering Pinapa callback !\n";

	LLVMContext & Context = getGlobalContext();
	// Allocate a full target machine description only if necessary.
	// FIXME: The choice of target should be controllable on the command line.
	std::auto_ptr < TargetMachine > target;

	std::string ErrorMessage;

	// Load the input module...
	std::auto_ptr < Module > M;
	if (MemoryBuffer * Buffer
	    = MemoryBuffer::getFileOrSTDIN(InputFilename, &ErrorMessage)) {
		M.reset(ParseBitcodeFile(Buffer, Context, &ErrorMessage));
		delete Buffer;
	}
	if (M.get() == 0) {
		if (ErrorMessage.size())
			errs() << ErrorMessage << "\n";
		else
			errs() << "bitcode didn't read correctly.\n";
		exit(1);
	}
	// Figure out what stream we are supposed to write to...
	// FIXME: outs() is not binary!
	raw_ostream *Out = &outs();	// Default to printing to stdout...
	if (OutputFilename != "-") {
		std::string ErrorInfo;
		Out =
		    new raw_fd_ostream(OutputFilename.c_str(), /*Binary= */
				       true,
				       false, ErrorInfo);
		if (!ErrorInfo.empty()) {
			errs() << ErrorInfo << '\n';
			delete Out;
			exit(1);
		}
		// Make sure that the Output file gets unlinked from the disk if we get a
		// SIGINT
		sys::RemoveFileOnSignal(sys::Path(OutputFilename));
	}
	// Create a PassManager to hold and optimize the collection of passes we are
	// about to build...
	//
	PassManager Passes;

	// Add an appropriate TargetData instance for this module...
	Passes.add(new TargetData(M.get()));

	// Check that the module is well formed on completion of optimization
	Passes.add(createVerifierPass());

	// Pinapa pass
	Passes.add(new FunctionNames());

	// Write bitcode out to disk or outs() as the last step...
	//Passes.add(createBitcodeWriterPass(*Out));

	// Now that we have all of the passes ready, run them.
	Passes.run(*M.get());

	// Delete the raw_fd_ostream.
	if (Out != &outs())
		delete Out;
}


int main(int argc, char **argv)
{
	llvm_shutdown_obj X;	// Call llvm_shutdown() on exit.

	llvm::cout << "Starting\n";

	try {

		// If we have a native target, initialize it to ensure it is linked in and
		// usable by the JIT.
		InitializeNativeTarget();

		cl::ParseCommandLineOptions(argc, argv,
					    "llvm .bc -> .bc modular optimizer and analysis printer\n");

		sys::PrintStackTraceOnErrorSignal();

		llvm::cout << "Starting SystemC elaboration...\n";
		launch_systemc(0, NULL);
	} catch(const std::string & msg) {
		errs() << argv[0] << ": " << msg << "\n";
	} catch(...) {
		errs() << argv[0] <<
		    ": Unexpected unknown exception occurred.\n";
	}
	llvm_shutdown();
	return 1;
}

char FunctionNames::ID = 42;
static RegisterPass < FunctionNames > X("pinapa", "Pinapa pass");
