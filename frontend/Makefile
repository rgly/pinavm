all: all-objs

SRCS = ${wildcard *.cpp}
TARGET = all

ROOT=..
include $(ROOT)/Makefile.common

CPPFLAGS += `llvm-config --cxxflags jit bitreader bitwriter native`
CPPFLAGS += -I$(SYSTEMC)/include

all-objs: $(OBJS)
#	g++ -shared $(OBJS) -o $@ $(SYSTEMCROOT)/lib-linux/libsystemc.a $(LDFLAGS) $(LDLIBS)
