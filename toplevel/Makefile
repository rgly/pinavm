SRCS = ${filter-out pinavm.cpp, ${wildcard *.cpp}}
TARGET_EXE = pinavm

all: $(TARGET_EXE)

ROOT=..
include $(ROOT)/Makefile.common

BACKENDSDIR = $(ROOT)/backends
FRONTENDDIR = $(ROOT)/frontend
UTILSDIR = $(ROOT)/utils

# We can't just link against libsystemc.a, since linking against a
# static library would take only the usefull symbols. pinavm doesn't
# use all of SystemC's symbols, and the platform may use some that we
# don't use directly => we have to take all .o files to make sure the
# symbols will be there when the elaboration will require it.
SYSTEMC_OBJS = ${shell find $(DOWNLOAD_AND_COMPILE_DIR)/systemc-2.2.0-gcc/objdir/ -name \*.o}

FRONTEND_CXX = ${wildcard $(ROOT)/frontend/*.cpp}
FRONTEND_SRC = $(FRONTEND_CXX) ${wildcard $(ROOT)/frontend/*.hpp}
BACKENDS_CXX = ${wildcard $(ROOT)/backends/**/*.cpp}
BACKENDS_SRC = $(BACKENDS_CXX) ${wildcard $(ROOT)/backends/**/*.hpp}
UTILS_CXX = ${wildcard $(ROOT)/utils/*.cpp}
UTILS_SRC = $(UTILS_CXX) ${wildcard $(ROOT)/utils/*.hpp}

FRONTEND_OBJS = $(FRONTEND_CXX:.cpp=.o)
BACKENDS_OBJS = $(BACKENDS_CXX:.cpp=.o)
UTILS_OBJS = $(UTILS_CXX:.cpp=.o)

ALL_OBJS = $(OBJS) $(UTILS_OBJS) $(FRONTEND_OBJS) $(BACKENDS_OBJS) $(SYSTEMC_OBJS)

CXXFLAGS += -I$(SYSTEMC)/include
CXXFLAGS += -I$(FRONTENDDIR)
CXXFLAGS += -I$(BACKENDSDIR)/SimpleBackend -I$(BACKENDSDIR)/PromelaBackend
CXXFLAGS += -I$(UTILSDIR)

# llvm-config sometimes includes -fno-rtti -fno-exceptions, but we don't want this:
# systemc.h is not compilable with -fno-rtti!
CXXFLAGS += `llvm-config --cxxflags jit bitreader bitwriter native` \
	-frtti -fexceptions

# These files define classes that inherit from LLVM classes, which are
# compiled with -fno-rtti as of svn version 98434 (14 mach 2010) while
# we need -frtti (see above), and deriving a rtti class from a no-rtti
# doesn't link. So, these two files are compiled without rtti.
CXXFLAGS.../backends/PromelaBackend/PromelaTargetMachine=-fno-rtti -fno-exceptions
CXXFLAGS.../backends/SimpleBackend/SimpleTargetMachine=-fno-rtti -fno-exceptions

LDLIBS += `llvm-config --ldflags --libs jit interpreter nativecodegen bitreader selectiondag`

frontend: $(FRONTEND_SRC) $(FRONTEND_OBJS)
	cd $(FRONTENDDIR) && $(MAKE) all

backends: $(BACKENDS_SRC) $(BACKENDS_OBJS)
	cd $(BACKENDSDIR) && $(MAKE) all

utils: $(UTILS_SRC) $(UTILS_OBJS)
	cd $(UTILSDIR) && $(MAKE) all

libpinavm.so: frontend backends utils $(OBJS)
	g++ -shared -o $@ $(ALL_OBJS) $(LDFLAGS) $(LDLIBS)

pinavm: pinavm.cpp libpinavm.so
	g++ -o $@ pinavm.cpp -lpinavm -L . -Wl,-rpath $(PWD)

llvm-version:
	llvm-config --version

debug-config:
	@echo CXXFLAGS=$(CXXFLAGS)
	@echo LDLIBS=$(LDLIBS)

clean:
	$(RM) $(OBJS) $(UTILS_OBJS) $(FRONTEND_OBJS) $(BACKENDS_OBJS)
