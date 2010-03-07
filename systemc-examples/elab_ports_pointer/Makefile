TARGET_ARCH=linux

SRC = main.cpp

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
CPPLLVMSCFLAGS=-I$(LLVMSYSTEMCROOT)/ -I$(LLVMSYSTEMCROOT)/include -I$(LLVMSYSTEMCROOT)/include/sysc
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
PINAPALIB=/home/marquet/git/llvm-front-end/toplevel/libpinapa.so
endif

ifndef SYSTEMCLIB
SYSTEMCLIB=$(INTACTSYSTEMCROOT)/lib-linux/libsystemc.a
endif

ifndef CPPSCFLAGS
CPPSCFLAGS=-I$(INTACTSYSTEMCROOT)/ -I$(INTACTSYSTEMCROOT)/include -I$(INTACTSYSTEMCROOT)/include/sysc
endif


GCC_SSA=${patsubst %.$(SUF),%.$(SUF).ssa,$(SRC)}
LLOPT=${patsubst %.ll,%.opt.ll,$(LL)}

all: main.exe main.opt.sc.bc

include ${SCOOT_HOME}/common.mk

ll: $(LL)
llopt: $(LLOPT)

gcc-ssa: $(GCC_SSA)

%.opt.sc.bc: %.opt.ll Makefile
	llvm-link -f -o $@ $(LLVMSYSTEMCROOT)/lib-$(TARGET_ARCH)/libsystemc.a $*.opt.bc 

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

# -fdump-tree-ssa works only if -O is provided.
%.$(SUF).ssa: %.$(SUF) Makefile ../include.mk
	$(COMP) -O $(CPPFLAGS) -c -fdump-tree-ssa $(INCLUDE) $< -o /dev/null
	mv $<.*.ssa $@

realclean: clean
	$(RM) *~ 

clean:
	-$(RM) *.exe *.bc *.ssa *.o
