#include <iostream>
#include <stdlib.h>
#include "libtest.h"

using namespace std;

void simple_in_native(base *b) {
	cout << "continue_in_native" << endl;
	b->hello();
	derived *d = dynamic_cast<derived *>(b);
	d->hello_derived();
}

void base::hello() {
	cout << "hello base" << endl;
};

void derived::hello() {
	cout << "hello derived" << endl;
};

void derived::hello_derived() {
	cout << "hello derived specific" << endl;
}

void target::hello_target () {
	//std::cout << "Hello2" << std::endl;
}

void base1_t::hello_t() {
	cout << "hello true" << endl;
}

template<>
void derived_t<true>::hello_derived_t() {
	cout << "hello true" << endl;
}

template<>
void derived_t<false>::hello_derived_t() {
	cout << "hello false" << endl;
}

void base2::hello2 () {
	std::cout << "Hello2" << std::endl;
}


void base_nv2::hello2 () {
	std::cout << "Hello NV2" << std::endl;
}

void common_base_in_native(base1 *b1, base2 *b2,
			   base_nv1 *nv1, base_nv2 *nv2,
			   base1_t *bt) {
	cout << "EXPECTED FAILURE: there is no virtual method for base1 in the .cpp file" << endl;
	TRY_CAST(b1, "JIT-ed base1", derived_common1);

	base1 *nb1 = new derived_common1();
	TRY_CAST(nb1, "Native base1", derived_common1);
	TRY_CAST(b2, "JIT-ed base2", target);

	cout << "EXPECTED FAILURE: there is no virtual method in derived_common1, although there is in a base class" << endl;
	TRY_CAST(b2, "JIT-ed base2", derived_common2);
	base2 *nb2 = new derived_common2();
	TRY_CAST(nb2, "Native base2", derived_common2);

	cout << "EXPECTED FAILURE: there is no virtual method in target_nv in the .cpp file" << endl;
	TRY_CAST(nv1, "JIT-ed nv1", target_nv);

	cout << "EXPECTED FAILURE: there is no virtual method in target_nv in the .cpp file" << endl;
	TRY_CAST(nv2, "JIT-ed nv2", target_nv);
	base_nv1 *nnv1 = new derived_common_nv1();
	TRY_CAST(nnv1, "Native-ed base_nv1", target_nv);
	
	cout << "EXPECTED FAILURE: if the target is a template, it fails, even though virtual methods are there in .cpp" << endl;
	TRY_CAST(bt, "JIT-ed bt", derived_t<true>);
}
