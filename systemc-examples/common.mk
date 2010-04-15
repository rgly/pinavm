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

ifndef CPPLLVMSCFLAGS
CPPLLVMSCFLAGS=-I$(INSTALL_PATH_SYSTEMC_LLVM)/ -I$(INSTALL_PATH_SYSTEMC_LLVM)/include -I$(INSTALL_PATH_SYSTEMC_LLVM)/include/sysc
endif

ifndef LLVMGCCFLAGS
LLVMGCCFLAGS=-fno-inline-functions $(CPPLLVMSCFLAGS)
endif

ifndef CPPFLAGS
CPPFLAGS=-fno-inline-functions
endif

ifndef OPTFLAGS
OPTFLAGS=-mem2reg -disable-opt -disable-inlining
endif

ifndef PINAPALIB
PINAPALIB=$(ROOT)/toplevel/libpinapa.so
endif

ifndef SYSTEMCLIB
SYSTEMCLIB=$(INSTALL_PATH_SYSTEMC_GCC)/lib-linux/libsystemc.a
endif

ifndef CPPSCFLAGS
CPPSCFLAGS=-I$(INSTALL_PATH_SYSTEMC_GCC)/ -I$(INSTALL_PATH_SYSTEMC_GCC)/include -I$(INSTALL_PATH_SYSTEMC_GCC)/include/sysc
endif


GCC_SSA=${patsubst %.$(SUF),%.$(SUF).ssa,$(SRC)}
LLOPT=${patsubst %.ll,%.opt.ll,$(LL)}

ifdef SCOOT_HOME
include ${SCOOT_HOME}/common.mk
endif

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

%.exe: %.$(SUF) Makefile $(PINAPALIB)
	$(COMP) $< -o $@ $(PINAPALIB) $(SYSTEMCLIB) $(CPPSCFLAGS) 

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
