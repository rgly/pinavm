# This is a utility which help user to create llvm bitcode easily
# while build time, It is recommended to use this function  with
# pinavm/util/LinkExternalBitcode.h

SET(LLVM_EXT bc)

function(build_llvm_bitcode f_target_name f_src_list)
  set(f_target_file ${CMAKE_CURRENT_BINARY_DIR}/${f_target_name}.${LLVM_EXT})
  set(f_compiled_target_file ${CMAKE_CURRENT_BINARY_DIR}/${f_target_name}.exe)

  # for each .cpp file, LLVMC would generate .s file in binary dir.
  foreach(f_temp_src ${${f_src_list}})
    get_filename_component(f_abso_temp_src ${f_temp_src} ABSOLUTE)

    STRING(REPLACE ".cpp" ".s" f_temp_object ${f_abso_temp_src})
    STRING(REPLACE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} 
		   f_temp_object ${f_temp_object})

    add_custom_command(OUTPUT ${f_temp_object} DEPENDS ${f_abso_temp_src}
      COMMAND ${LLVM_COMPILER} ${LLVMC_FLAGS} ${f_abso_temp_src}
      -o ${f_temp_object}
      VERBATIM)

    LIST(APPEND f_objects ${f_temp_object})
  endforeach(f_temp_src)


  add_custom_command(OUTPUT ${f_target_file} DEPENDS ${f_objects}
    COMMAND ${LLVM_LINK} ${f_objects} -o ${f_target_file} 
    VERBATIM)

  add_custom_command(OUTPUT ${f_compiled_target_file}
    DEPENDS ${f_target_file} systemc_lib tlm-basic
    COMMAND ${LLVM_COMPILER} ${f_target_file}
    -o ${f_compiled_target_file} -L ${CMAKE_BINARY_DIR}
    -L ${CMAKE_BINARY_DIR}/external/basic -ltlm-basic -lsystemc_lib -lpthread VERBATIM)

  add_custom_target(${f_target_name} DEPENDS ${f_target_file})
  add_custom_target(${f_target_name}.exe DEPENDS ${f_compiled_target_file})
endfunction(build_llvm_bitcode)

