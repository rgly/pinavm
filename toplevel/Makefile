SRCS = ${wildcard *.cpp}
TARGET = libpinapa.so
all: $(TARGET)

ROOT=..
include $(ROOT)/Makefile.common

FRONTEND_OBJS = ${wildcard $(SYSTEMCFRONTEND)/*.o}
BACKENDS_OBJS = ${wildcard $(LLVMBACKENDS)/*.o}
SYSTEMC_OBJS = $(SYSTEMCROOT)/lib-linux/libsystemc.a

FRONTEND_SRC = ${wildcard $(SYSTEMCFRONTEND)/*.cpp} ${wildcard $(SYSTEMCFRONTEND)/*.hpp}
BACKENDS_SRC = ${wildcard $(LLVMBACKENDSHEADERS)/*.cpp} ${wildcard $(LLVMBACKENDSHEADERS)/*.hpp}

ALL_OBJS = $(OBJS) $(FRONTEND_OBJS) $(BACKENDS_OBJS) $(SYSTEMC_OBJS)

CPPFLAGS += -I$(SYSTEMC)/include
CPPFLAGS += -I$(SYSTEMCFRONTEND)
CPPFLAGS += -I$(LLVMBACKENDSHEADERS)
CPPFLAGS += `llvm-config --cxxflags jit bitreader bitwriter native`

LDLIBS += `llvm-config --ldflags --libs jit bitreader bitwriter native` 

frontend: $(FRONTEND_SRC)
	cd $(SYSTEMCFRONTEND) && make all

backends: $(BACKENDS_SRC)
	cd $(LLVMBACKENDS) && make

$(TARGET): frontend backends $(OBJS)
	g++ -shared -o $@ $(ALL_OBJS) $(LDFLAGS) $(LDLIBS)
