SRC = main.cpp

all:  main.opt.sc.bc
.PHONY: all promela diff

include ../common.mk

run: main.bc
	../../toplevel/pinavm -print-ir -print-elab -b promela -o ok.3.pr main.bc -inline
