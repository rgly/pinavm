#include <systemc.h>

//using namespace sc_core;



SC_MODULE(Writer)
{
public:

	sc_out<bool> out;

			
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
	sc_in<bool> in;

	Reader() : sc_module("reader"), in("in"){SC_THREAD(main);}

	SC_CTOR(Reader): in("in") {
		SC_THREAD(main);
	}
	
	void main() {
		while (in.read() != true) {

			wait(5, SC_NS);

		}
	}
};

#define NB_CHAINS 2

int sc_main (int argc , char *argv[]) 
{
	int i;
	sc_signal<bool> s[NB_CHAINS];
	
 
	//sc_signal<bool> s;
	//Writer Cin;
	//Reader Cout;
	Writer Cin[2];
	Reader Cout[2];
	//Cin[0].out(s[0]);
	//Cout[0].in(s[0]);
	//Cin[0].out(s[0]);
	//Cout[0].in(s[0]);
	for (i = 0 ; i < 2 ; i++) {
		Cin[i].out(s[i]);
		Cout[i].in(s[i]);
	}

	//Run the Simulation for "200 nanosecnds"
	sc_start(200, SC_NS);

	return 0;
}
