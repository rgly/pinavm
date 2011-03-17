#ifndef LIBTEST_H
#define LIBTEST_H

void test_function (int normal, int weak, int linkonce,
		    int *normal_addr, int *weak_addr, int *linkonce_odr);

#endif // LIBTEST_H
