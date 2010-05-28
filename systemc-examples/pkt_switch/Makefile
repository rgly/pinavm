OVERRIDING=default

SRCS = main.cpp fifo.cpp receiver.cpp sender.cpp switch_clk.cpp switch.cpp

all: final.bc
#promela: ok.pr
.PHONY: all diff

HEADERS = fifo.h pkt.h receiver.h sender.h switch_clk.h switch.h switch_reg.h
INCLUDE + = $HEADERS
BITCODES = main.opt.bc fifo.opt.bc receiver.opt.bc sender.opt.bc switch_clk.opt.bc switch.opt.bc

include ../common.mk

frontend:$(PINAVM) main.opt.bc
	../../toplevel/pinavm main.opt.bc --print-ir --print-elab

final.pr: final.bc $(SRCS) $(HEADERS) $(BITCODES)
	-$(RM) $@.bak
	-mv $@ $@.bak
	../../toplevel/pinavm -print-ir -print-elab -b promela -o final.pr final.bc -inline -args $(ARG)

final.bc: $(BITCODES) $(HEADERS) $(SRCS)
	llvm-ld -b=$@ $(BITCODES)

diff:
	diff -u final.pr final.pr.bak

promela: final.pr 
	echo running with $(ARG) and $(OVERRIDING)
