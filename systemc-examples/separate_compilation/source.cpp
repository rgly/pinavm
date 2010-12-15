#include "source.h"

void Source::compute() {
	out.write(true); // <----
}
Source::Source(sc_module_name name) : sc_module(name) {
	printf("Building Source\n");
	SC_THREAD(compute);
}
