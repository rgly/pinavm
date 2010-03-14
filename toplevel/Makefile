SRCS = ${wildcard *.cpp}
TARGET = libpinapa.so
TARGET_EXE = pinavm
all: $(TARGET)
both: $(TARGET) $(TARGET_EXE)

ROOT=..
include $(ROOT)/Makefile.common

BACKENDSDIR = $(ROOT)/backends
FRONTENDDIR = $(ROOT)/frontend
UTILSDIR = $(ROOT)/utils

SYSTEMC_OBJS = $(SYSTEMCROOT)/lib-linux/libsystemc.a

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

# llvm-config sometimes includes -fno-rtti -fno-exceptions, but we don't want this.
CXXFLAGS += `llvm-config --cxxflags jit bitreader bitwriter native` \
	-frtti -fexceptions

LDLIBS += `llvm-config --ldflags --libs jit interpreter nativecodegen bitreader selectiondag`

frontend: $(FRONTEND_SRC) $(FRONTEND_OBJS)
	cd $(FRONTENDDIR) && $(MAKE) all

backends: $(BACKENDS_SRC) $(BACKENDS_OBJS)
	cd $(BACKENDSDIR) && $(MAKE) all

utils: $(UTILS_SRC) $(UTILS_OBJS)
	cd $(UTILSDIR) && $(MAKE) all

$(TARGET): frontend backends utils $(OBJS)
	g++ -shared -o $@ $(ALL_OBJS) $(LDFLAGS) $(LDLIBS)


main-load-bc.o: main.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) -DLOAD_PLATFORM_BC $(CXXEXTRAFLAGS)

$(TARGET_EXE): frontend backends utils main-load-bc.o $(filter-out main.o, $(OBJS))
	g++ -o $@ $(filter-out main.o, $(ALL_OBJS)) main-load-bc.o $(LDFLAGS) $(LDLIBS)

llvm-version:
	llvm-config --version

debug-config:
	@echo CXXFLAGS=$(CXXFLAGS)
	@echo LDLIBS=$(LDLIBS)

clean:
	$(RM) $(OBJS) $(UTILS_OBJS) $(FRONTEND_OBJS) $(BACKENDS_OBJS)
