SRCS = ${wildcard *.cpp}
TARGET = libpinapa.so
all: $(TARGET)

ROOT=..
include $(ROOT)/Makefile.common

BACKENDSDIR = $(ROOT)/backends
FRONTENDDIR = $(ROOT)/frontend
UTILSDIR = $(ROOT)/utils

FRONTEND_OBJS = ${wildcard $(FRONTENDDIR)/*.o}
BACKENDS_OBJS = ${wildcard $(BACKENDSDIR)/SimpleBackend/*.o} ${wildcard $(BACKENDSDIR)/PromelaBackend/*.o}
UTILS_OBJS = ${wildcard $(UTILSDIR)/*.o}

SYSTEMC_OBJS = $(SYSTEMCROOT)/lib-linux/libsystemc.a

FRONTEND_SRC = ${wildcard $(ROOT)/frontend/*.cpp} ${wildcard $(ROOT)/frontend/*.hpp}
BACKENDS_SRC = ${wildcard $(ROOT)/backends/**/*.cpp} ${wildcard $(ROOT)/backends/**/*.hpp}
UTILS_SRC = ${wildcard $(ROOT)/utils/*.cpp} ${wildcard $(ROOT)/utils/*.hpp}

ALL_OBJS = $(OBJS) $(UTILS_OBJS) $(FRONTEND_OBJS) $(BACKENDS_OBJS) $(SYSTEMC_OBJS)

CPPFLAGS += -I$(SYSTEMC)/include
CPPFLAGS += -I$(FRONTENDDIR)
CPPFLAGS += -I$(BACKENDSDIR)/SimpleBackend -I$(BACKENDSDIR)/PromelaBackend
CPPFLAGS += -I$(UTILSDIR)
CPPFLAGS += `llvm-config --cxxflags jit bitreader bitwriter native`

LDLIBS += `llvm-config --ldflags --libs jit bitreader bitwriter native` 

frontend: $(FRONTEND_SRC)
	cd $(FRONTENDDIR) && make all

backends: $(BACKENDS_SRC)
	cd $(BACKENDSDIR) && make all

utils: $(UTILS_SRC)
	cd $(UTILSDIR) && make all

$(TARGET): frontend backends utils $(OBJS)
	g++ -shared -o $@ $(ALL_OBJS) $(LDFLAGS) $(LDLIBS)
