SPIN = spin
CC = gcc
TARGET_ARCH=linux

OVERRIDING=default

ifndef ROOT
ROOT=../../
endif

include $(ROOT)/config.mk

ifndef QUIET_MODE
QUIET_MODE=yes
endif

ifeq (QUIET_MODE,yes)
REDIRECT=>$@.log 2>$@.log
endif

ifndef SUF
SUF=cpp
endif

ifndef COMP
COMP=g++
endif

CFLAGS=-DSAFETY

ifndef SRC
SRC=${wildcard *.$(SUF)}
endif

ifndef LL
LL=${patsubst %.$(SUF),%.ll,$(SRC)}
endif

ifndef EXE
EXE=${patsubst %.$(SUF),%.exe,$(SRC)}
endif

ifndef PROMELA
PROMELA=${patsubst %.$(SUF),%.pr,$(SRC)}
endif

ifndef PINAVM_ARGS
PINAVM_ARGS=-print-ir -print-elab $(PINAVM_EXTRA_ARGS) $(PINAVM_LIBS:%=-load %)
endif

.PHONY: promela diff frontend
promela: $(PROMELA)

diff:
	diff -u $(PROMELA) $(PROMELA).bak

frontend: main.opt.bc
	@$(MAKE) $(PINAVM)
	$(PINAVM) main.opt.bc $(PINAVM_ARGS) -args $(ARG)

pan.c: $(PROMELA)
	$(SPIN) -a $(PROMELA) 

pan: pan.c
	$(CC) $(CFLAGS) $< -o $@

spin-run: pan
	./pan

ifndef CPPFLAGS
CPPFLAGS=-fno-inline-functions -g
endif

ifndef OPTFLAGS
OPTFLAGS=-mem2reg -disable-opt -disable-inlining
endif

ifndef SYSTEMCLIB
SYSTEMCLIB=$(INSTALL_PATH_SYSTEMC_GCC)/lib-linux/libsystemc.a
endif

ifndef SYSTEMC_INCLUDE
SYSTEMC_INCLUDE=-I$(INSTALL_PATH_SYSTEMC_GCC)/include
endif

ifndef CPPSCFLAGS
CPPSCFLAGS=-I$(ROOT)/external/systemc-2.2.0/src/
endif

ifndef LLVMGCCFLAGS
LLVMGCCFLAGS=-fno-inline-functions $(CPPSCFLAGS)
endif

GCC_SSA=${patsubst %.$(SUF),%.$(SUF).ssa,$(SRC)}
LLOPT=${patsubst %.ll,%.opt.ll,$(LL)}

ifdef SCOOT_HOME
include ${SCOOT_HOME}/common.mk
endif

PINAVM=$(ROOT)/toplevel/pinavm
# convenience target to be able to type "make pinavm"
pinavm: $(PINAVM)

.PHONY: $(PINAVM) pinavm
$(PINAVM):
	cd $$(dirname $(PINAVM)) && $(MAKE) $$(basename $(PINAVM))


#final.pr: final.bc $(SRCS) $(HEADERS) $(BITCODES)
#	-$(RM) $@.bak
#	-mv $@ $@.bak
#	../../toplevel/pinavm -print-ir -print-elab -b promela -o final.pr final.bc -inline -args $(ARG)

#final.bc: $(BITCODES) $(HEADERS) $(SRCS)
#	llvm-ld -b=$@ $(BITCODES)

#diff:
#	diff -u final.pr final.pr.bak

#promela: final.pr 
#	echo running with $(ARG) and $(OVERRIDING)

ll: $(LL)
llopt: $(LLOPT)

gcc-ssa: $(GCC_SSA)

%.opt.sc.bc: %.opt.bc Makefile
	llvm-link -f -o $@ $(INSTALL_PATH_SYSTEMC_LLVM)/lib-$(TARGET_ARCH)/libsystemc.a $*.opt.bc

%.opt.sc.ll: %.opt.sc.bc Makefile
	llvm-dis -f $*.opt.sc.bc -o $@

%.opt.bc: %.bc Makefile
	opt -f $(OPTFLAGS) $*.bc -o $*.opt.bc

%.opt.ll: %.opt.bc Makefile
	llvm-dis -f $*.opt.bc -o $@

%.ll: %.bc Makefile
	llvm-dis -f $*.bc -o $*.ll

%.bc: %.$(SUF) Makefile
	llvm-$(COMP) $(LLVMGCCFLAGS) -emit-llvm -c $< -o $@ $(INCLUDE)

%.simu: %.$(SUF) Makefile
	$(COMP) $< -o $@ $(SYSTEMCLIB) $(CPPFLAGS) $(SYSTEMC_INCLUDE)

%.pr: %.opt.bc Makefile
# Keep a backup of the target.
	-$(RM) $@.bak
	-if [ -f $@ ]; then mv $@ $@.bak; fi
# Make sure $(PINAVM) is up-to-date. Since $(PINAVM) is a .PHONY
# target, adding it as a dependancy would trigger inconditional re-run
# of PinaVM, so we add a recursive call instead.
	@$(MAKE) $(PINAVM)
	@echo running with $(ARG) and $(OVERRIDING);
# If pinavm fails, make sure we don't keep a half-build .pr file around, so that next
# "make promela" runs also fail.
	$(PINAVM) $(PINAVM_ARGS) -b promela -o $@.part main.opt.bc -inline -args $(ARG) $(REDIRECT)
	@mv $@.part $@

kascpar: $(SRC)
	sc2xml -f $(SRC) -i $(HEADERS) -o main_kascpar.cpp

quiny: $(SRC)
	g++ -I $(QUINYINCLUDE) -L. -o quiny-res $(SRC) -L$(QUINYLIB) -l qsystemc

xml:
	doxygen ../Doxyfile
	systemcxml --dtd $(SYSTEMCXML_HOME)/systemc.dtd xspinml/main_8cpp.xml

# -fdump-tree-ssa works only if -O is provided.
%.$(SUF).ssa: %.$(SUF) Makefile ../include.mk
	$(COMP) -O $(CPPFLAGS) -c -fdump-tree-ssa $(INCLUDE) $< -o /dev/null
	mv $<.*.ssa $@

clean:
	-$(RM) *.exe *.bc *.ll *.o *.log

realclean: clean promelaclean
	$(RM) *~

promelaclean:
	-$(RM) pan $(PROMELA) $(PROMELA).trail $(PROMELA).bak pan.c pan.h pan.b pan.m pan.t
