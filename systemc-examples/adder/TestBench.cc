#include "TestBench.h"


TestBench::TestBench(sc_module_name name) : sc_module(name) {
	SC_THREAD(genIn);
}


void TestBench::genIn() {
	a.write(1);
	b.write(1);
	wait(10, SC_NS);
	while (true) {
		wait(10, SC_NS);
		a.write(a.read() + 1);
		b.write(b.read() + 1);
		cout << c.read() << endl;
	}
}
