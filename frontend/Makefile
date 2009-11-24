LLVM_CPPFLAGS = `llvm-config --cxxflags jit bitreader bitwriter native`
LLVM_LDLIBS = `llvm-config --ldflags --libs jit bitreader bitwriter native` 

MODULE = pinapa

SRCS = ${wildcard *.cpp}
TARGET = libpinapa.so

include ./Makefile.common

$(TARGET): $(OBJS)
	g++ -shared $(OBJS) -o $@ $(SYSTEMCROOT)/lib-linux/libsystemc.a $(LDFLAGS) $(LDLIBS)
