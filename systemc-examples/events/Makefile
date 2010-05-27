
SRC = main.cpp

all: main.bc
promela: final.pr
.PHONY: all promela diff

include ../common.mk

frontend:$(PINAVM) main.opt.bc
	../../toplevel/pinavm main.opt.bc --print-ir --print-elab


final.pr: main.opt.bc
# Keep a backup of the target.
	-$(RM) $@.bak
	-mv $@ $@.bak
	echo running with $(ARG) and $(OVERRIDING);
	../../toplevel/pinavm -print-ir -print-elab -b promela -o final.pr main.opt.bc -inline -args $(ARG)

diff:
	diff -u ok.pr ok.pr.bak
