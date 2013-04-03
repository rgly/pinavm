SPIN = spin
CC = gcc
TARGET_ARCH=linux

OVERRIDING=default

ifndef BINARY_ROOT
BINARY_ROOT=../..
endif

include $(BINARY_ROOT)/config.mk

ifndef PRINT_IR
PRINT_IR=yes
endif
ifndef PRINT_ELAB
PRINT_ELAB=yes
endif

ifeq ($(PRINT_IR),yes)
PRINT_IR_MAYBE=-print-ir
else
PRINT_IR_MAYBE=
endif

ifeq ($(PRINT_ELAB),yes)
PRINT_ELAB_MAYBE=-print-elab
else
PRINT_ELAB_MAYBE=
endif


ifndef QUIET_MODE
QUIET_MODE=disable
endif

# disable error messages by default
ifeq ($(QUIET_MODE),disable)
DISABLE_DEBUG_MAYBE=-dis-dbg-msg
else
DISABLE_DEBUG_MAYBE=
endif

# make QUIET_MODE=redirect bla-bla to redirect output to file.
ifeq ($(QUIET_MODE),redirect)
REDIRECT=>$@.log 2>&1
endif

ifndef SUF
SUF=cpp
endif

ifndef COMP
COMP=g++
endif

CFLAGS=-DSAFETY

# define SRC for single source, and SRCS for separate compilation.
ifndef SRC
ifndef SRCS
SRCS=${wildcard *.$(SUF)}
endif
endif

# Bitcode file after running mem2reg

ifndef PINAVM_INPUT_BC
ifdef SRC
PINAVM_INPUT_BC=${patsubst %.$(SUF),%.bc,$(SRC)}
else
PINAVM_INPUT_BC=main.linked.bc
endif
endif

ifndef PINAVM_INPUT_BC_M2R
PINAVM_INPUT_BC_M2R = ${patsubst %.bc, %.opt.bc, $(PINAVM_INPUT_BC)}
endif

debug:
	echo $(SRC)
	echo $(SRCS)
	echo $(PINAVM_INPUT_BC)
	echo $(PINAVM_INPUT_BC_M2R)

ifndef LL
LL=${patsubst %.$(SUF),%.ll,$(SRC)}
endif

ifndef EXE
EXE=${patsubst %.$(SUF),%.exe,$(SRC)}
endif

ifndef PROMELA
ifdef SRC
PROMELA=${patsubst %.$(SUF),%.pr,$(SRC)}
else
PROMELA=main.pr
endif
endif

ifndef PINAVM_ARGS
PINAVM_ARGS=$(PRINT_IR_MAYBE) $(PRINT_ELAB_MAYBE) \
	 $(PINAVM_EXTRA_ARGS) $(PINAVM_LIBS:%=-load %) $(DISABLE_DEBUG_MAYBE)
endif

ifdef ARG
ARG_MAYBE=$(ARG)
else
ARG_MAYBE=
endif

.PHONY: promela diff frontend
promela: $(PROMELA)

diff:
	diff -u $(PROMELA) $(PROMELA).bak

frontend: $(PINAVM_INPUT_BC_M2R)
	@$(MAKE) $(PINAVM)
	$(PINAVM) $(PINAVM_ARGS) $(PINAVM_INPUT_BC_M2R) $(ARG_MAYBE)

tweto: $(PINAVM_INPUT_BC) pinavm
	$(PINAVM) -b tweto $(PINAVM_ARGS) $(PINAVM_INPUT_BC) $(ARG_MAYBE) $(REDIRECT)

run: $(PINAVM_INPUT_BC) pinavm
	$(PINAVM) -b run $(PINAVM_ARGS) $(PINAVM_INPUT_BC) $(ARG_MAYBE) $(REDIRECT)

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


GCC_SSA=${patsubst %.$(SUF),%.$(SUF).ssa,$(SRC)}
LLOPT=${patsubst %.ll,%.opt.ll,$(LL)}

ifdef SCOOT_HOME
include ${SCOOT_HOME}/common.mk
endif

PINAVM=$(BINARY_ROOT)/toplevel/pinavm
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

%.opt.bc: %.bc Makefile
	opt -f $(OPTFLAGS) $*.bc -o $*.opt.bc

ifdef SRCS
$(PINAVM_INPUT_BC): $(SRCS:.cpp=.bc)
	llvm-link $^ -o $@
endif

%.opt.ll: %.opt.bc Makefile
	llvm-dis -f $*.opt.bc -o $@

%.ll: %.bc Makefile
	llvm-dis -f $*.bc -o $*.ll

%.bc: %.$(SUF) Makefile
	$(LLVM_COMPILER) $(LLVMC_FLAGS) $< -o $@ $(INCLUDE)

%.simu: %.$(SUF) Makefile
	$(COMP) $< -o $@ $(SYSTEMCLIB) $(CPPFLAGS) $(SYSTEMC_INCLUDE)

%.pr: $(PINAVM_INPUT_BC_M2R) Makefile
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
	$(PINAVM) $(PINAVM_ARGS) -b promela -o $@.part -inline $(PINAVM_INPUT_BC_M2R) $(ARG_MAYBE) $(REDIRECT)
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
