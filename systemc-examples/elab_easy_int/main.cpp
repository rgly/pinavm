#include <systemc.h>

//using namespace sc_core;
//#define KASCPAR

#define NS * 1e-9

SC_MODULE(Component)
{
public:
#ifndef KASCPAR
	sc_core::sc_out<int> out;
	sc_core::sc_in<int> in;
#else	
 	sc_out<int> out;
 	sc_in<int> in;
#endif

	bool notified;
	bool isHead;
		
	void process();

	SC_CTOR(Component):
		out("Out"),
		in("in")
		{
			notified = false;
			isHead = false;
			SC_THREAD(main);
		}
	
 	void essaiwrite();

	int example(int x) {
		int res = x + 42;
		return res;
	}

	void main() {
		if (isHead == false) {
			while (in.read() != 5) {
//				sc_core::wait(5, SC_NS);
	                        wait(5, SC_NS);
			}
		}

		example(0);
		notified = true;
		essaiwrite();
	}
	
	
};

void Component::essaiwrite()
{
	out.write(5);
}

int sc_main (int argc , char *argv[]) 
{
#ifndef KASCPAR
	sc_core::sc_signal<int> s1("s1"), s2("s2"), s3("s3");
#else
	sc_signal<int> s1("s1"), s2("s2"), s3("s3");
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

	C1.isHead = true;

	//Run the Simulation for "200 nanosecnds"
	sc_start(200, SC_NS);

	return 0;
}
