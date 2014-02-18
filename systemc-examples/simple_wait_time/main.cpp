#include <systemc.h>

SC_MODULE(CONPONENT)
{
	SC_CTOR(CONPONENT) {
		SC_THREAD(process);
	}
	
	void process()
	{
		while(1) {
			wait(20,SC_NS);
			cout << "every 20ns\n";
		}
	}
};



int sc_main (int argc , char *argv[]) 
{
	CONPONENT co("Conponent");
	//Run the Simulation for "200 nanosecnds"
	sc_start(200, SC_NS);

	return 0;
}
