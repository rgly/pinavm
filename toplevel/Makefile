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

# Object list is hardcoded. It's the only way to have the list of
# objects reliably before the first compilation.
SYSTEMC_OBJS = ../external/systemc-2.2.0/objdir-gcc/src/sysc/tracing/sc_wif_trace.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/tracing/sc_trace.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/tracing/sc_vcd_trace.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/utils/sc_stop_here.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/utils/sc_list.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/utils/sc_mempool.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/utils/sc_report.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/utils/sc_pq.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/utils/sc_utils_ids.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/utils/sc_hash.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/utils/sc_report_handler.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/utils/sc_string.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/communication/sc_event_finder.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/communication/sc_export.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/communication/sc_event_queue.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/communication/sc_semaphore.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/communication/sc_signal_resolved_ports.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/communication/sc_signal_resolved.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/communication/sc_signal.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/communication/sc_signal_ports.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/communication/sc_port.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/communication/sc_interface.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/communication/sc_clock.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/communication/sc_prim_channel.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/communication/sc_mutex.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/qt/qtmdc.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/qt/qt.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/qt/qtmds.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/int/sc_nbutils.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/int/sc_nbdefs.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/int/sc_signed.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/int/sc_int64_io.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/int/sc_length_param.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/int/sc_int32_mask.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/int/sc_uint_base.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/int/sc_int64_mask.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/int/sc_unsigned.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/int/sc_int_base.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/int/sc_nbexterns.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/fx/scfx_pow10.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/fx/sc_fxnum_observer.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/fx/sc_fxnum.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/fx/sc_fxval.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/fx/sc_fxtype_params.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/fx/sc_fxdefs.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/fx/scfx_utils.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/fx/sc_fxcast_switch.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/fx/scfx_mant.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/fx/scfx_rep.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/fx/sc_fxval_observer.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/misc/sc_concatref.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/misc/sc_value_base.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/bit/sc_bit.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/bit/sc_bv_base.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/bit/sc_lv_base.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/bit/sc_logic.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_object.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_cor_fiber.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_join.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_module.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_sensitive.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_thread_process.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_attribute.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_module_registry.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_module_name.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_method_process.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_reset.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_cthread_process.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_wait.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_cor_pthread.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_object_manager.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_main_main.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_name_gen.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_ver.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_main.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_simcontext.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_wait_cthread.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_process.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_event.o \
	       ../external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_time.o

# We let make check whether a rebuild is needed or not before
# launching a recursive $(MAKE). This allows the main target to do
# re-link pinavm only when some source file (SystemC or not) is
# needed. For that, a .PHONY target wouldn't do (it's considered to be
# never up to date), but a timestamp file together with the explicit
# list of sources works fine.
SYSTEMC_SRC = ${shell find ../external/systemc-2.2.0/src/ -type f}
SYSTEMC_STAMP = ../external/systemc-2.2.0/objdir-gcc/systemc-timestamp.txt

$(SYSTEMC_STAMP): $(SYSTEMC_SRC)
	@echo "Building SystemC ..."
	@if [ ! -f ../external/systemc-2.2.0/objdir-gcc/Makefile ]; then \
		cd ../external/systemc-2.2.0/ && ./autogen.sh && \
		mkdir -p objdir-gcc/ && cd objdir-gcc/ && \
		../configure ;\
	fi
	cd ../external/systemc-2.2.0/objdir-gcc/ && $(MAKE) pthreads_debug
	date > ../external/systemc-2.2.0/objdir-gcc/systemc-timestamp.txt

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

CXXFLAGS += -I../external/systemc-2.2.0/src/
CXXFLAGS += -I$(FRONTENDDIR)
CXXFLAGS += -I$(BACKENDSDIR)/SimpleBackend -I$(BACKENDSDIR)/PromelaBackend -I$(BACKENDSDIR)/42Backend
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
CXXFLAGS.../backends/42Backend/42TargetMachine=-fno-rtti -fno-exceptions

LDLIBS += `llvm-config --ldflags --libs jit interpreter nativecodegen bitreader selectiondag`

# libpinavm.so: $(FRONTEND_OBJS) $(BACKENDS_OBJS) $(UTILS_OBJS) $(OBJS)
# 	g++ -shared -o $@ $(ALL_OBJS) $(LDFLAGS) $(LDLIBS)

pinavm: pinavm.cpp $(SYSTEMC_STAMP) $(FRONTEND_OBJS) $(BACKENDS_OBJS) $(UTILS_OBJS) $(OBJS)
	g++ -rdynamic -o $@ pinavm.cpp $(ALL_OBJS) $(LDFLAGS) $(LDLIBS)

llvm-version:
	llvm-config --version

debug-config:
	@echo CXXFLAGS=$(CXXFLAGS)
	@echo LDLIBS=$(LDLIBS)

clean:
	$(RM) $(OBJS) $(UTILS_OBJS) $(FRONTEND_OBJS) $(BACKENDS_OBJS)
