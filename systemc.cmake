
# We can't just link against libsystemc.a, since linking against a
# static library would take only the usefull symbols. pinavm doesn't
# use all of SystemC's symbols, and the platform may use some that we
# don't use directly => we have to take all .o files to make sure the
# symbols will be there when the elaboration will require it.

# Object list is hardcoded. It's the only way to have the list of
# objects reliably before the first compilation.
SET(SYSTEMC_OBJS ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/tracing/sc_wif_trace.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/tracing/sc_trace.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/tracing/sc_vcd_trace.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/utils/sc_stop_here.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/utils/sc_list.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/utils/sc_mempool.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/utils/sc_report.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/utils/sc_pq.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/utils/sc_utils_ids.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/utils/sc_hash.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/utils/sc_report_handler.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/utils/sc_string.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/communication/sc_event_finder.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/communication/sc_export.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/communication/sc_event_queue.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/communication/sc_semaphore.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/communication/sc_signal_resolved_ports.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/communication/sc_signal_resolved.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/communication/sc_signal.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/communication/veri_signal.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/communication/sc_signal_ports.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/communication/sc_port.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/communication/sc_interface.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/communication/sc_clock.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/communication/sc_prim_channel.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/communication/sc_mutex.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/qt/qtmdc.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/qt/qt.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/qt/qtmds.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/int/sc_nbutils.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/int/sc_nbdefs.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/int/sc_signed.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/int/sc_int64_io.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/int/sc_length_param.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/int/sc_int32_mask.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/int/sc_uint_base.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/int/sc_int64_mask.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/int/sc_unsigned.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/int/sc_int_base.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/int/sc_nbexterns.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/fx/scfx_pow10.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/fx/sc_fxnum_observer.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/fx/sc_fxnum.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/fx/sc_fxval.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/fx/sc_fxtype_params.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/fx/sc_fxdefs.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/fx/scfx_utils.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/fx/sc_fxcast_switch.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/fx/scfx_mant.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/fx/scfx_rep.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/fx/sc_fxval_observer.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/misc/sc_concatref.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/misc/sc_value_base.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/bit/sc_bit.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/bit/sc_bv_base.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/bit/sc_lv_base.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/datatypes/bit/sc_logic.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_object.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_cor_fiber.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_join.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_module.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_sensitive.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_thread_process.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_attribute.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_module_registry.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_module_name.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_method_process.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_reset.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_cthread_process.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_wait.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_cor_pthread.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_object_manager.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_main_main.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_name_gen.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_ver.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_main.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_simcontext.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_wait_cthread.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_process.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_event.o
	         ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/src/sysc/kernel/sc_time.o)

# We let make check whether a rebuild is needed or not before
# launching a recursive $(MAKE). This allows the main target to do
# re-link pinavm only when some source file (SystemC or not) is
# needed. For that, a .PHONY target wouldn't do (it's considered to be
# never up to date), but a timestamp file together with the explicit
# list of sources works fine.
execute_process(COMMAND find external/systemc-2.2.0/src/ -type f
  OUTPUT_VARIABLE SYSTEMC_SRC)
STRING(REGEX REPLACE "\n" ";" SYSTEMC_SRC "${SYSTEMC_SRC}")

SET(SYSTEMC_STAMP ${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/objdir-gcc/systemc-timestamp.txt)

STRING(REGEX REPLACE ";" " " SYSTEMC_OBJS_SPC "${SYSTEMC_OBJS}")

# Add SystemC's .o files manually on the command-line. A bit hackish,
# but the .o files are generated files that escape CMake's control, so
# they're not really source files, and not really generated files
# either.
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SYSTEMC_OBJS_SPC}")

add_custom_command(OUTPUT ./external/systemc-2.2.0/objdir-gcc/systemc-timestamp.txt
  DEPENDS ${SYSTEMC_SRC}
  COMMAND echo "Building SystemC ..."
  COMMAND MAKE=$(MAKE) ./build-systemc.sh
  COMMAND date > ./external/systemc-2.2.0/objdir-gcc/systemc-timestamp.txt)

ADD_CUSTOM_TARGET(systemc-all ALL DEPENDS ./external/systemc-2.2.0/objdir-gcc/systemc-timestamp.txt)
