#include <systemc.h>
#include <sstream>
using namespace std;

#define NB_COMPONENTS 4

SC_MODULE(DEMULTIPLEXER)
{
	sc_out<bool> output[NB_COMPONENTS];
//	sc_core::sc_out<int> ticks[NB_COMPONENTS];

	SC_CTOR(DEMULTIPLEXER) {
		SC_THREAD(ind);
	}
	
	void ind() {
		int i;

		output[0] = true;
		output[1] = true;
		output[2] = true;
		output[3] = true;

// 		for (i = 1 ; i <= NB_COMPONENTS ; i++) {
// 			output[NB_COMPONENTS - i].write((i+1) * 2);
// 		}
	}

	void process()
	{
		ind();
	}
};


SC_MODULE(COMPONENT)
{
	sc_signal<bool> s;
	sc_in<bool> data;
	bool dr;

	void ind() {
		dr = data.read();
		cout << "\n";
		cout << dr << "\n";

	}

	void process() {
		ind();
	}

	SC_CTOR(COMPONENT) {
 		data(s);
 		SC_METHOD(ind);
 		sensitive << data.default_event();
		sc_module::dont_initialize();
	}
};

int sc_main (int argc , char *argv[]) 
{
	DEMULTIPLEXER dm("Demultiplexer");
	int i;
	sc_signal<bool> s1("s1"), s2("s2"), s3("s3"), s4("s4");
	COMPONENT C1("C1"), C2("C2"), C3("C3"), C4("C4");
	char buf[10];
       
	int first=0, second=1, third=2, fourth=3;

//  	istringstream(argv[1]) >> first;
//  	istringstream(argv[2]) >> second;
//  	istringstream(argv[3]) >> third;
//  	istringstream(argv[4]) >> fourth;

//  	dm.output[first](C1.s);
//  	dm.output[second](C2.s);
//  	dm.output[third](C3.s);
//  	dm.output[fourth](C4.s);

 	dm.output[0](C1.s);
 	dm.output[1](C2.s);
 	dm.output[2](C3.s);
 	dm.output[3](C4.s);


	//Run the Simulation for "200 nanosecnds"
	sc_start(200, SC_NS);

	return 0;
}
