SRC = main.cpp

ROOT=../../../../
all: main.bc

include $(ROOT)/systemc-examples/common.mk

ok.42: main.bc
	$(PINAVM) -print-ir -print-elab -b 42 -o $@ main.bc
