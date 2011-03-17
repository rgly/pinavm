run: main.bc libtest.so
	lli -load=./libtest.so main.bc 

main.bc: main.cpp libtest.h
	llvm-g++ -c -emit-llvm $< -o $@

libtest.so: libtest.cpp libtest.h
	g++ -fpic -shared $< -o $@
