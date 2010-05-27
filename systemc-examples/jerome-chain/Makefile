SRC = main.cpp
SRCS = $(SRC)
BITCODES = main.opt.bc
HEADERS = 
INCLUDE + = $HEADERS

VAR = 1

all: main.bc
promela: final.pr
.PHONY: all promela diff

include ../common.mk

frontend:$(PINAVM) main.opt.bc
	../../toplevel/pinavm main.opt.bc --print-ir --print-elab

final.pr: main.opt.bc
#	Keep a backup of the target.
	-$(RM) $@.bak
	-mv $@ $@.bak
	../../toplevel/pinavm -print-ir -print-elab -b promela -o final.pr main.opt.bc -inline -args $(ARG)

diff:
	diff -u ok.pr ok.pr.bak



