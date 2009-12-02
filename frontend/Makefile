SRCS = ${wildcard *.cpp}
TARGET = all

include ../Makefile.common

CPPFLAGS += `llvm-config --cxxflags jit bitreader bitwriter native`
CPPFLAGS += -I$(SYSTEMC)/include

all: $(OBJS)
#	g++ -shared $(OBJS) -o $@ $(SYSTEMCROOT)/lib-linux/libsystemc.a $(LDFLAGS) $(LDLIBS)
