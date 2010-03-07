#include <systemc.h>

//using namespace sc_core;

#define NS * 1e-9
#define KASCPAR

SC_MODULE(Component)
{
public:
#ifndef KASCPAR
	sc_core::sc_out<bool> out;
	sc_core::sc_in<bool> in;
#else
	sc_out<bool> out;
	sc_in<bool> in;
#endif
	int end;

	SC_CTOR(Component):
		out("Out"),
		in("in")
		{
			end = 0;
			SC_THREAD(main_proc);
		}
	
 	void essaiwrite();

	void main_proc() {
		while (end != 100) {
#ifndef KASCPAR
			sc_core::wait(5, SC_NS);
#else
			wait(5, SC_NS);
#endif
			out.write(true);
		}
//		sc_stop();
	}
};

int sc_main (int argc , char *argv[])
{
#ifndef KASCPAR
	sc_core::sc_signal<bool> s1("s1"), s2("s2"), s3("s3");
#else
	sc_signal<bool> s1("s1"), s2("s2"), s3("s3");
#endif
// 	sc_signal<bool> s1;
// 	sc_signal<bool> s2;
// 	sc_signal<bool> s3;
	Component C1("C1");
	Component C2("C2");
	Component C3("C3");
       
	C1.out(s1);
	C2.out(s2);
	C3.out(s3);

	C1.in(s3);
	C2.in(s1);
	C3.in(s2);

	//Run the Simulation for "200 nanosecnds"
	sc_start(200, SC_NS);

	return 0;
}
