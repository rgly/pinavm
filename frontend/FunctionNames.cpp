#include <systemc>

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
#include "SCElab.h"
#include "config.h"

#include "sysc/kernel/sc_process_table.h"
#include "sysc/kernel/sc_process_handle.h"
#include "sysc/kernel/sc_simcontext.h"
#include "sysc/kernel/sc_thread_process.h"
#include "sysc/kernel/sc_module_registry.h"

#include <iostream>
#include <typeinfo>

using namespace llvm;

extern int launch_systemc(int argc, char *argv[]);


static cl::opt < std::string >
InputFilename(cl::Positional, cl::desc("<input bitcode file>"),
	      cl::init("-"), cl::value_desc("filename"));

static cl::opt < std::string >
OutputFilename("o", cl::desc("Override output filename"),
               cl::value_desc("filename"), cl::init("-"));

struct FunctionNames : public ModulePass {
  SCJit* scjit;
  SCCFactory* sccfactory;

public:
  static char ID; // Pass identification, replacement for typeid
  FunctionNames() : ModulePass(&ID) { }

  
  ~FunctionNames()
  {
//     delete this->sccfactory;
  }
  
  bool runOnModule(Module &M) {
    Module* llvmMod = &M;
    Function* F;
    sc_core::sc_thread_handle  thread_p;  // Pointer to thread process accessing.

    TRACE_1("Getting ELAB\n");

    // To call the "end_of_elaboration()" methods.
    sc_core::sc_get_curr_simcontext()->initialize(true);
    
    SCElab* elab = new SCElab(llvmMod);
    std::vector<Function*>* fctStack = new std::vector<Function*>();
    
    //------- Get modules and ports --------
    vector<sc_core::sc_module*> modules = sc_core::sc_get_curr_simcontext()->get_module_registry()->m_module_vec;
    vector<sc_core::sc_module*>::iterator modIt;
    
    for (modIt = modules.begin() ; modIt < modules.end() ; modIt++) {
      sc_core::sc_module* mod = *modIt;
      
      IRModule* m = elab->addModule(mod);
      std::vector<sc_core::sc_port_base*>* ports = mod->m_port_vec;

      vector<sc_core::sc_port_base*>::iterator it;
      for (it = ports->begin() ; it < ports->end() ; it++) {
	sc_core::sc_port_base* p = *it;
	elab->addPort(m, p);
      }
    }
    
    //------- Get processes and events --------
    sc_core::sc_process_table * processes = sc_core::sc_get_curr_simcontext()->m_process_table;
    for ( thread_p = processes->thread_q_head(); 
	  thread_p; thread_p = thread_p->next_exist() )
      {
	sc_core::sc_process_b* theProcess = thread_p;	
	sc_core::sc_module* mod = (sc_core::sc_module*) thread_p->m_semantics_host_p;
	IRModule* m = elab->getIRModule(mod);
	Process* process = elab->addProcess(m, theProcess);
	
	std::vector<const sc_core::sc_event*> eventsVector = theProcess->m_static_events;
	vector<const sc_core::sc_event*>::iterator it;
	for (it = eventsVector.begin() ; it < eventsVector.end() ; it++) {
	  sc_core::sc_event* ev = (sc_core::sc_event*) *it; 
	  elab->addEvent(process, ev);
	}
      }
    
    this->scjit = new SCJit(llvmMod, elab);
    this->sccfactory = new SCCFactory(scjit);
    
    TRACE_1("Analyzing code\n");
    
    // Walk through call graph and build intermediate representation
    vector<Process*>::iterator processIt = elab->getProcesses()->begin();
    vector<Process*>::iterator endIt = elab->getProcesses()->end();
    
    for(; processIt < endIt ; processIt++) {
      Process* proc = *processIt;
      fctStack->push_back(proc->getMainFct());
      this->scjit->setCurrentProcess(proc);
      while (! fctStack->empty()) {
	F = fctStack->back();
	fctStack->pop_back();
	TRACE_3("Parsing Function : " << F->getNameStr() << "\n");
	for (Function::iterator bb = F->begin(), be = F->end(); bb != be; ++bb) { 
	  BasicBlock::iterator i = bb->begin(), ie = bb->end();
	  while (i != ie) {
	    CallInst* callInst = dyn_cast<CallInst>(&*i);
	    if (callInst) {
	      if (! sccfactory->handle(F, &*bb, callInst)) {
		TRACE_4("Call not handled : " << callInst->getCalledFunction()->getNameStr() << "\n");
		fctStack->push_back(callInst->getCalledFunction());
	      }
	    }
	    BasicBlock::iterator tmpend = bb->end();
	    i++;
	  }
	}
      }
    }

    elab->printIR(sccfactory);
    
    this->scjit->doFinalization();
    delete fctStack;
    delete this->scjit;
    delete elab;
    return false;
  }
  
  void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
  }
};

void
pinapa_callback()
{
  TRACE_1("Entering Pinapa, building module\n");  
  Module* M;

  LLVMContext &Context = getGlobalContext();
  
  std::string ErrorMessage;
  
  // Load the input module...
  //  Module* mod = new Module("Frontend");
  //  std::auto_ptr<Module> M;
  // Make the module, which holds all the code.

  if (MemoryBuffer *Buffer
      = MemoryBuffer::getFileOrSTDIN(InputFilename, &ErrorMessage)) {
    M = ParseBitcodeFile(Buffer, Context, &ErrorMessage);
    delete Buffer;
  } else {
    ERROR("Not able to initialize module from bitcode\n");
  }
    
  // Figure out what stream we are supposed to write to...
  // FIXME: outs() is not binary!
  raw_ostream *Out = &outs();  // Default to printing to stdout...
  if (OutputFilename != "-") {
    std::string ErrorInfo;
    Out = new raw_fd_ostream(OutputFilename.c_str(), /*Binary=*/true,
			     false, ErrorInfo);
    if (!ErrorInfo.empty()) {
      delete Out;
      ERROR(ErrorInfo << "\n");
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
  TargetData* td = new TargetData(M);
  Passes.add(td);
  
  // Check that the module is well formed on completion of optimization
  FunctionPass* vp = createVerifierPass();
  Passes.add(vp);
  
  // Pinapa pass
  FunctionNames* fn = new FunctionNames();
  Passes.add(fn);
  
  // Write bitcode out to disk or outs() as the last step...
  //Passes.add(createBitcodeWriterPass(*Out));
  
  // Now that we have all of the passes ready, run them.
  Passes.run(*M);

  TRACE_1("Shutdown\n");

  llvm_shutdown();

  //  delete M;
  
  // Delete the raw_fd_ostream.
  if (Out != &outs())
    delete Out;
}

int main(int argc, char **argv)
{
  llvm_shutdown_obj X;  // Call llvm_shutdown() on exit.
  
  try {

    // If we have a native target, initialize it to ensure it is linked in and
    // usable by the JIT.
    InitializeNativeTarget();
    
    cl::ParseCommandLineOptions(argc, argv,
				"llvm .bc -> .bc modular optimizer and analysis printer\n");
    
    sys::PrintStackTraceOnErrorSignal();

    TRACE_1("Executing SystemC elaboration\n");  
    launch_systemc(0, NULL);
  } catch (const std::string& msg) {
    errs() << argv[0] << ": " << msg << "\n";
  } catch (...) {
    errs() << argv[0] << ": Unexpected unknown exception occurred.\n";
  }
  llvm_shutdown();
  return 1;
}

char FunctionNames::ID = 42;
static RegisterPass < FunctionNames > X("pinapa", "Pinapa pass");
