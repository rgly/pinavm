SRC = main.cpp

all: main.bc
promela: ok.3.pr
.PHONY: all promela diff

include ../common.mk

ok.%.pr: main.bc
# Keep a backup of the target.
	-$(RM) $@.bak
	-mv $@ $@.bak
	../../toplevel/pinavm -print-ir -print-elab -b promela -o ok.3.pr main.bc -inline -args $*

diff:
	diff -u ok.3.pr ok.3.pr.bak
