SRC = main.cpp

all: main.exe main.opt.sc.bc
run-pinavm:
.PHONY: all run-pinavm

include ../common.mk

ok.%.pr: main.exe main.opt.sc.bc
	./main.exe -print-ir -print-elab -b promela -o ok.3.pr main.opt.sc.bc -inline -args $*

run-pinavm: main.opt.sc.bc
	../../toplevel/pinapa main.opt.sc.bc
