// Options available for backends.
// Add new options if you need.
// 
#include <string>

namespace sc_core {
    class sc_simcontext;
    class sc_time;
}

namespace llvm {
    class ExecutionEngine;
}

class BackendOption {
	public:
		std::string OutputFilename;
		// Encode events using booleans instead of integers
		bool EventsAsBool;
		// Use relative clocks in the backend
		bool RelativeClocks;
		// Insert bug int the last process (in the file generated by the backend
		bool Bug;
		sc_core::sc_simcontext* context;
		// SystemC duration by sc_start()
		const sc_core::sc_time* duration;
		// Disable debug messages
		bool DisableMsg;
		// Disable debug messages from the optimizer
		bool DisableOptDbgMsg;

};
