#include <systemc>

using namespace sc_core;

struct sc_verif
{
	static void azerty(bool cond) {
		//abort();
		;
	}
};

class Component : public sc_module {
public:
	void compute() {
		sc_verif::azerty(false);
		;
	}
	SC_CTOR(Component) {
		SC_THREAD(compute);
	}
};

int sc_main(int argc, char **argv) {
	Component c("c");
	sc_start();
}
