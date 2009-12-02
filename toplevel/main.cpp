#include <systemc>

#include <string>

#include "llvm/Support/CommandLine.h"
#include "llvm/System/Signals.h"

#include "FrontendItf.hpp"
#include "SimpleBackend.hpp"

static cl::opt < std::string >
InputFilename(cl::Positional, cl::desc("<input bitcode file>"),
	      cl::init("-"), cl::value_desc("filename"));

static cl::opt < std::string >
OutputFilename("o", cl::desc("Override output filename"),
	       cl::value_desc("filename"), cl::init("-"));

static cl::opt < std::string >
Backend("b", cl::desc("Backend to use"),
	cl::value_desc("backend-name"), cl::init("-"));

static cl::opt < bool >
PrintElab("print-elab", cl::desc("Print architecture after elaboration"));

static cl::opt < bool >
PrintIR("print-ir",
	cl::desc("Print Intermediate representation for all processes"));

void pinapa_callback()
{
	TRACE_1("Entering Pinapa, building module\n");

	Frontend *fe = launch_frontend(InputFilename);

	if (PrintIR) {
		fe->printIR();
	}
	if (PrintElab) {
		fe->printElab("");
	}

	if (Backend != "-") {
		if (Backend == "simple" || Backend == "Simple") {
			launch_simplebackend(fe, OutputFilename);
		} else {
			ERROR("Backend " << Backend << " unknown\n");
		}
	}

	TRACE_1("Shutdown...\n");
	llvm_shutdown();
}



int main(int argc, char **argv)
{
	llvm_shutdown_obj X;	// Call llvm_shutdown() on exit.

	try {

		// If we have a native target, initialize it to ensure it is linked in and
		// usable by the JIT.
		InitializeNativeTarget();

		cl::ParseCommandLineOptions(argc, argv,
					    "llvm .bc -> .bc modular optimizer and analysis printer\n");

		sys::PrintStackTraceOnErrorSignal();

		TRACE_1("Executing SystemC elaboration\n");
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
