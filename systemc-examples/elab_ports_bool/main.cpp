#include <systemc.h>

#define KASCPAR

SC_MODULE(DEMULTIPLEXER)
{
#ifndef KASCPAR
	sc_core::sc_out<bool> output[4];
#else
	sc_out<bool> output[4];
#endif
//	sc_core::sc_out<int> ticks[NB_COMPONENTS];

	SC_CTOR(DEMULTIPLEXER) {
		SC_THREAD(process);
	}
	
	void process()
	{
		output[0].write(true);
		output[1].write(true);
		output[2].write(false);
		output[3].write(true);
	}
};


SC_MODULE(COMPONENT)
{
#ifndef KASCPAR
	sc_core::sc_in<bool> data;
#else
	sc_in<bool> data;
#endif
	bool dr;

	SC_CTOR(COMPONENT) : data("data") {
		SC_THREAD(process);
		sensitive << data;
		this->dr = 0;
	}
	
	void process() {
//		wait(data->default_event());
		dr = data.read();
	}
};

int sc_main (int argc , char *argv[]) 
{
	DEMULTIPLEXER dm("Demultiplexer");
	int i;
	sc_signal<bool> s1("s1"), s2("s2"), s3("s3"), s4("s4");
	COMPONENT C1("C1"), C2("C2"), C3("C3"), C4("C4");

	C1.data(s1);
	C2.data(s2);
	C3.data(s3);
	C4.data(s4);

	dm.output[0](s1);
 	dm.output[1](s2);
 	dm.output[2](s3);
 	dm.output[3](s4);

	//Run the Simulation for "200 nanosecnds"
	sc_start(200, SC_NS);

	return 0;
}
