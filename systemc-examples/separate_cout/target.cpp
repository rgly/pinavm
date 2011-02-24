#include "target.h"

void Target::compute() {
	int i = 0;
	std::cout << "Target starts" << std::endl;
	while (in.read() == true && i < 10) {
		std::cout << "Target waits" << std::endl;
		wait(0, SC_NS);
		i++;
	}
	if (i != 10)
		sc_verif::ASSERT(false);
}

Target::Target(sc_module_name name) : sc_module(name) {
	printf("Building Target\n");
	SC_THREAD(compute);
}
