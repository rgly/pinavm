all: minimal.g++-4.4.exe minimal.g++-4.2.exe minimal.llvm-g++.exe minimal.gcc-ssa-native.exe

include ../config.mk

ifndef SYSTEMC_HOME
# unmodified SystemC.
SYSTEMC_HOME = /home/moy/local/usr/src/systemc-2.2.0/
endif

.PHONY: ../config.mk
../config.mk:
	@cd ../ && $(MAKE) config.mk --no-print-directory -s

minimal.%.o: minimal.cpp
	$* $< -c -o $@ -I $(SYSTEMC_HOME)/include

minimal.gcc-ssa-native.o: minimal.cpp
	llvm-g++ -c --emit-llvm $< -o $<.bc -I $(SYSTEMC_HOME)/include
	llc -f $<.bc -o $<.s
	as $<.s -o $@

minimal.%.exe: minimal.%.o
	g++ $< $(SYSTEMC_HOME)/lib-linux/libsystemc.a -o $@

.PHONY: clean
clean:
	$(RM) *.o *.bc *.exe *.s
