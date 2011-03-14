#include "libtest.h"
#include <stdio.h>

extern int weak_odr_symbol;
extern int linkonce_odr_symbol;
extern int normal_symbol;

int weak_odr_symbol = 43;
int linkonce_odr_symbol = 12;
int normal_symbol = 41;

void test_function (int normal, int weak, int linkonce,
		    int *normal_addr, int *weak_addr, int *linkonce_addr) {
	printf("test_function called\n");
	printf("weak_odr_symbol = %d\n", weak_odr_symbol);
	printf("&weak_odr_symbol = %p\n", &weak_odr_symbol);
	printf("weak = %d\n", weak);
	printf("weak_addr = %p\n", weak_addr);

	printf("normal_symbol = %d\n", normal_symbol);
	printf("&normal_symbol = %p\n", &normal_symbol);
	printf("normal = %d\n", normal);
	printf("normal_addr = %p\n", normal_addr);

	printf("linkonce_odr_symbol = %d\n", linkonce_odr_symbol);
	printf("&linkonce_odr_symbol = %p\n", &linkonce_odr_symbol);
	printf("linkonce = %d\n", linkonce);
	printf("linkonce_addr = %p\n", linkonce_addr);
}

