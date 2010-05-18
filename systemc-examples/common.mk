TARGET_ARCH=linux

ifndef ROOT
ROOT=../../
endif

include $(ROOT)/config.mk

ifndef SUF
SUF=cpp
endif

ifndef COMP
COMP=g++
endif

ifndef SRC
SRC=${wildcard *.$(SUF)}
endif

ifndef LL
LL=${patsubst %.$(SUF),%.ll,$(SRC)}
endif

ifndef EXE
EXE=${patsubst %.$(SUF),%.exe,$(SRC)}
endif

ifndef CPPFLAGS
CPPFLAGS=-fno-inline-functions
endif

ifndef OPTFLAGS
OPTFLAGS=-mem2reg -disable-opt -disable-inlining
endif

ifndef SYSTEMCLIB
SYSTEMCLIB=$(INSTALL_PATH_SYSTEMC_GCC)/lib-linux/libsystemc.a
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
.PHONY: $(PINAVM)
$(PINAVM):
	cd $$(dirname $(PINAVM)) && $(MAKE) $$(basename $(PINAVM))


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

%.bc: %.$(SUF) Makefile $(PINAVM)
	llvm-$(COMP) $(LLVMGCCFLAGS) -emit-llvm -c $< -o $@ $(INCLUDE)

%.simu: %.$(SUF) Makefile
	$(COMP) $< -o $@ $(SYSTEMCLIB) $(CPPSCFLAGS)

kascpar: $(SRC)
	sc2xml -f $(SRC) -i $(HEADERS) -o main_kascpar.cpp

quiny: $(SRC)
	g++ -I $(QUINYINCLUDE) -L. -o quiny-res $(SRC) -L$(QUINYLIB) -l qsystemc

xml:
	doxygen ../Doxyfile
	systemcxml --dtd $(SYSTEMCXML_HOME)/systemc.dtd xml/main_8cpp.xml

# -fdump-tree-ssa works only if -O is provided.
%.$(SUF).ssa: %.$(SUF) Makefile ../include.mk
	$(COMP) -O $(CPPFLAGS) -c -fdump-tree-ssa $(INCLUDE) $< -o /dev/null
	mv $<.*.ssa $@

clean:
	-$(RM) *.exe *.bc *.ssa *.o

realclean: clean
	$(RM) *~ 
