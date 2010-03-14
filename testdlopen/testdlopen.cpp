/********************************************************************
 * Copyright (C) 2010 by Verimag                                    *
 * Initial author: Matthieu Moy                                     *
 * This file must not be distributed outside Verimag                *
 ********************************************************************/

/*!
  \file testdlopen.cpp
  \brief Test program for dlopen(NULL, ...);
*/

#include <dlfcn.h>
#include <iostream>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

extern "C" int my_function_to_load () {
	return 42;
}

extern "C" int my_integer;

int my_integer = 42;

int main() {
	char *error;
	cout << my_function_to_load() << ", " << my_integer << endl;

#define ERR (error = dlerror(), error ? error : "OK")
#define TEST_DLOPEN(file, flags)		do {		\
	cout << "dlopen(" << file << ", " #flags ");" << endl;	\
	void	*H    = dlopen(file, flags);	\
	if (!H) {				\
		cout << "Can't load file" << dlerror() << endl;	\
		abort();			\
	}					\
	void	*ptrF = dlsym(H, "my_function_to_load");	\
	cout << "my_function_to_load = " << ptrF << " : " << ERR << endl; \
	void	*ptrI = dlsym(H, "my_integer");	\
	cout << "my_integer = " << ptrI << " : " << ERR << endl; \
	void	*ptrP = dlsym(H, "printf");	\
	cout << "printf = " << ptrP << " : " << ERR << endl; \
	void	*ptrO = dlsym(H, "open");	\
	cout << "open = " << ptrO << " : " << ERR << endl;	\
	} while(0)

	TEST_DLOPEN(NULL, RTLD_LAZY);
	TEST_DLOPEN("./libtestdlopen.so", RTLD_LAZY);
	TEST_DLOPEN("./testdlopen", RTLD_LAZY);
}



