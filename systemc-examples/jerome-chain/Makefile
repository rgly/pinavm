SRC = main.cpp

all: main.exe main.opt.sc.bc
promela: ok.3.pr
.PHONY: all promela

include ../common.mk

ok.%.pr: main.exe main.opt.sc.bc
	./main.exe -print-ir -print-elab -b promela -o ok.3.pr main.opt.sc.bc -inline -args $*
