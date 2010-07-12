OVERRIDING=default

SRCS = main.cpp fifo.cpp receiver.cpp sender.cpp switch_clk.cpp switch.cpp
PROMELA = final.pr
ARG=3

all: promela
.PHONY: all

HEADERS = fifo.h pkt.h receiver.h sender.h switch_clk.h switch.h switch_reg.h
INCLUDE + = $HEADERS
BITCODES = main.opt.bc fifo.opt.bc receiver.opt.bc sender.opt.bc switch_clk.opt.bc switch.opt.bc

include ../common.mk

final.bc: $(BITCODES) $(HEADERS) $(SRCS)
	llvm-ld -b=$@ $(BITCODES)

