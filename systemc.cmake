
SET(SYSTEMC_SOURCE_DIR "${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/src/sysc" )

# We can't just link against libsystemc.a, since linking against a
# static library would take only the usefull symbols. pinavm doesn't
# use all of SystemC's symbols, and the platform may use some that we
# don't use directly => we have to take all .o files to make sure the
# symbols will be there when the elaboration will require it.

# Object list is hardcoded. It's the only way to have the list of
# objects reliably before the first compilation.
SET(SYSTEMC_CPP_SRC
	         tracing/sc_wif_trace
	         tracing/sc_trace
	         tracing/sc_vcd_trace
	         utils/sc_stop_here
	         utils/sc_list
	         utils/sc_mempool
	         utils/sc_report
	         utils/sc_pq
	         utils/sc_utils_ids
	         utils/sc_hash
	         utils/sc_report_handler
	         utils/sc_string
	         communication/sc_event_finder
	         communication/sc_export
	         communication/sc_event_queue
	         communication/sc_semaphore
	         communication/sc_signal_resolved_ports
	         communication/sc_signal_resolved
	         communication/sc_signal
	         communication/veri_signal
	         communication/sc_signal_ports
	         communication/sc_port
	         communication/sc_interface
	         communication/sc_clock
	         communication/sc_prim_channel
	         communication/sc_mutex
	         datatypes/int/sc_nbutils
	         datatypes/int/sc_nbdefs
	         datatypes/int/sc_signed
	         datatypes/int/sc_int64_io
	         datatypes/int/sc_length_param
	         datatypes/int/sc_int32_mask
	         datatypes/int/sc_uint_base
	         datatypes/int/sc_int64_mask
	         datatypes/int/sc_unsigned
	         datatypes/int/sc_int_base
	         datatypes/int/sc_nbexterns
	         datatypes/fx/scfx_pow10
	         datatypes/fx/sc_fxnum_observer
	         datatypes/fx/sc_fxnum
	         datatypes/fx/sc_fxval
	         datatypes/fx/sc_fxtype_params
	         datatypes/fx/sc_fxdefs
	         datatypes/fx/scfx_utils
	         datatypes/fx/sc_fxcast_switch
	         datatypes/fx/scfx_mant
	         datatypes/fx/scfx_rep
	         datatypes/fx/sc_fxval_observer
	         datatypes/misc/sc_concatref
	         datatypes/misc/sc_value_base
	         datatypes/bit/sc_bit
	         datatypes/bit/sc_bv_base
	         datatypes/bit/sc_lv_base
	         datatypes/bit/sc_logic
	         kernel/sc_object
	         kernel/sc_cor_fiber
	         kernel/sc_join
	         kernel/sc_module
	         kernel/sc_sensitive
	         kernel/sc_thread_process
	         kernel/sc_attribute
	         kernel/sc_module_registry
	         kernel/sc_module_name
	         kernel/sc_method_process
	         kernel/sc_reset
	         kernel/sc_cthread_process
	         kernel/sc_wait
	         kernel/sc_cor_pthread
	         kernel/sc_object_manager
	         kernel/sc_main_main
	         kernel/sc_name_gen
	         kernel/sc_ver
	         kernel/sc_main
	         kernel/sc_simcontext
	         kernel/sc_wait_cthread
	         kernel/sc_process
	         kernel/sc_event
	         kernel/sc_time
	         kernel/sc_cor_qt )


# Extending ${SYSTEMC_CPP_SRC} to full file names like
FOREACH(systemc_short_file_name ${SYSTEMC_CPP_SRC})
  SET(SYSTEMC_OBJS ${SYSTEMC_OBJS} "${SYSTEMC_SOURCE_DIR}/${systemc_short_file_name}.cpp")
ENDFOREACH(systemc_short_file_name)

# Since QuickThread contains assembly code for different platform.
# Detect the Host Architecture and assign assembly code.
TRY_RUN(run_result compile_result "${CMAKE_BINARY_DIR}/tmp"
    "${CMAKE_SOURCE_DIR}/scripts/check_host_architecture.cpp"
    RUN_OUTPUT_VARIABLE QTMDS )

FOREACH(qt_obj "md/${QTMDS}.s" qt.c md/null.c )
  SET(temp_qt_obj "${SYSTEMC_SOURCE_DIR}/qt/${qt_obj}")
	 
  # Since gxx can compile assembly code, we treat *.s file as CXX code.
  # (I do not know why my CMake not recognize LANGUAGE C)
  SET_PROPERTY(SOURCE ${temp_qt_obj} PROPERTY LANGUAGE CXX)
  #  Quick Thread sources to be included.
  SET(SYSTEMC_OBJS ${SYSTEMC_OBJS} ${temp_qt_obj}   )
ENDFOREACH(qt_obj)

add_library(systemc_lib OBJECT ${SYSTEMC_OBJS})


