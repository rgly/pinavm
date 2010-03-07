#include <systemc.h>

class Component : public sc_module
{
public:
// 	sc_core::sc_out<bool> out;
// 	sc_core::sc_in<bool> in;
	
	sc_out<bool> out;
	sc_in<bool> in;
	
	bool notified;
	bool isHead;
		
	void process();

	SC_HAS_PROCESS(Component);

	Component(sc_module_name name) : sc_module(name) {
		SC_THREAD(main);
		notified = false;
		isHead = false;
	}

	void main();
};
