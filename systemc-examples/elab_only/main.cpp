#include <systemc.h>

//using namespace sc_core;

//#define KASCPAR

//#define NS * 1e-9

SC_MODULE(Component)
{
public:
#ifdef KASCPAR
	sc_out<bool> out;
	sc_in<bool> in;
#else
	sc_core::sc_out<bool> out;
	sc_core::sc_in<bool> in;
#endif

	SC_CTOR(Component): out("Out"),	in("in") { }	
};

int sc_main (int argc , char *argv[]) 
{
#ifndef KASCPAR
	sc_core::sc_signal<bool> s1("s1");
	sc_core::sc_signal<bool> s2("s2");
	sc_core::sc_signal<bool> s3("s3");
#else
	sc_signal<bool> s1("s1"), s2("s2"), s3("s3");
#endif

	Component C1("C1"), C2("C2"), C3("C3");
       
	C1.out(s1);
	C2.out(s2);
	C3.out(s3);

	C1.in(s3);
	C2.in(s1);
	C3.in(s2);

//	C1.isHead = true;

	//Run the Simulation for "200 nanosecnds"
	sc_start(200, SC_NS);

	return 0;
}
