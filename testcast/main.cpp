#include <iostream>

using namespace std;

#include "libtest.h"

int main () {
	cout << "entering main" << endl;
	base *b = new derived();
	b->hello();
	derived *d = dynamic_cast<derived *>(b);
	d->hello_derived();
	simple_in_native(b);

	base1 *b1 = new derived_common1();
	base2 *b2 = new derived_common2();
	base_nv1 *nv1 = new derived_common_nv1();
	base_nv2 *nv2 = new derived_common_nv2();
	base1_t *bt = new derived_t<true>();
	TRY_CAST(nv1, "JIT-ed nv1 within JIT", target_nv);
	TRY_CAST(nv2, "JIT-ed nv2 within JIT", target_nv);
	TRY_CAST(b1, "JIT-ed base1 within JIT", derived_common1);
	TRY_CAST(b2, "JIT-ed base2 within JIT", target);
	common_base_in_native(b1, b2, nv1, nv2, bt);
}
