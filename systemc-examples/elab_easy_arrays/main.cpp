#include <systemc.h>

//using namespace sc_core;

//#define NS * 1e-9
//#define KASCPAR

SC_MODULE(Writer)
{
public:
#ifndef KASCPAR
	sc_core::sc_out<bool> out;
#else
	sc_out<bool> out;
#endif
			
	Writer() : sc_module("writer"), out("Out"){SC_THREAD(main);}

	SC_CTOR(Writer): out("Out") {
		SC_THREAD(main);
	}

	void main() {
		out.write(true);
	}
};

SC_MODULE(Reader)
{
public:
#ifndef KASCPAR
	sc_core::sc_in<bool> in;
#else
	sc_in<bool> in;
#endif
	Reader() : sc_module("reader"), in("in"){SC_THREAD(main);
}

	SC_CTOR(Reader): in("in") {
		SC_THREAD(main);
	}
	
	void main() {
		while (in.read() != true) {
#ifndef KASCPAR
			sc_core::wait(5, SC_NS);
#else
			wait(5, SC_NS);
#endif
		}
	}
};

#define NB_CHAINS 2

int sc_main (int argc , char *argv[]) 
{
	int i;
#ifndef KASCPAR
	sc_core::sc_signal<bool> s[NB_CHAINS];
#else
	sc_signal<bool> s[NB_CHAINS];
#endif
	Writer Cin[NB_CHAINS];
	Reader Cout[NB_CHAINS];

	for (i = 0 ; i < NB_CHAINS ; i++) {
		Cin[i].out(s[i]);
		Cout[i].in(s[i]);
	}

	//Run the Simulation for "200 nanosecnds"
	sc_start(200, SC_NS);

	return 0;
}
