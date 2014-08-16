# This is a utility which help user to create llvm bitcode easily
# while build time, It is recommended to use this function  with
# pinavm/util/LinkExternalBitcode.h

SET(LLVM_EXT bc)

macro(collect_included_headers f_abso_temp_src)
  # collect the header dependency of source code.
  execute_process(COMMAND 
    ${LLVM_COMPILER} ${LLVMC_FLAGS} ${f_abso_temp_src} -MM
    OUTPUT_VARIABLE f_dep_var
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  # clang -MM generate Makefile dep, so string truncation is necessery.
  if(NOT "${f_dep_var}" STREQUAL "")
    # remove the backslash and newline
    string(REPLACE "\\\n" ";" f_dep_var "${f_dep_var}")
    string(REPLACE " "    ""  f_dep_var "${f_dep_var}")
    # remove the first item, which normally be "main.o : main.cpp"
    LIST(REMOVE_AT f_dep_var 0)
  endif()
endmacro(collect_included_headers)

macro(build_llvm_bc_object f_temp_src)
  get_filename_component(f_abso_temp_src ${f_temp_src} ABSOLUTE)

  STRING(REPLACE ".cpp" ".s" f_temp_object ${f_abso_temp_src})
  STRING(REPLACE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} 
      	   f_temp_object ${f_temp_object})

  # the macro to create header dependency.
  collect_included_headers(${f_abso_temp_src})

  LIST(APPEND f_dep_var ${f_abso_temp_src})

  add_custom_command(OUTPUT ${f_temp_object} DEPENDS ${f_dep_var}
    COMMAND ${LLVM_COMPILER} ${LLVMC_FLAGS} ${f_abso_temp_src}
    -o ${f_temp_object} VERBATIM)

  LIST(APPEND f_objects ${f_temp_object})
endmacro(build_llvm_bc_object)

function(build_llvm_bitcode f_target_name f_src_list)
  set(f_target_unopt_file
             ${CMAKE_CURRENT_BINARY_DIR}/${f_target_name}.unopt.${LLVM_EXT})

  set(f_target_file ${CMAKE_CURRENT_BINARY_DIR}/${f_target_name}.${LLVM_EXT})
  set(f_compiled_target_file ${CMAKE_CURRENT_BINARY_DIR}/${f_target_name}.exe)
  set(f_staticopt_bc_target_file ${CMAKE_CURRENT_BINARY_DIR}/output.bc)
  set(f_staticopt_target_file ${CMAKE_CURRENT_BINARY_DIR}/${f_target_name}.opt)

  # for each .cpp file, LLVMC would generate .s file in binary dir.
  foreach(f_temp_src ${${f_src_list}})
    build_llvm_bc_object(${f_temp_src})
  endforeach(f_temp_src)


  add_custom_command(OUTPUT ${f_target_unopt_file} DEPENDS ${f_objects}
    COMMAND ${LLVM_LINK} ${f_objects} -o ${f_target_unopt_file} 
    VERBATIM)

  add_custom_command(OUTPUT ${f_target_file} DEPENDS ${f_target_unopt_file}
    COMMAND ${LLVM_OPT} -mem2reg -o ${f_target_file} ${f_target_unopt_file} 
    VERBATIM)

  add_custom_command(OUTPUT ${f_compiled_target_file}
    DEPENDS ${f_target_file} systemc_lib tlm-basic
    COMMAND ${LLVM_COMPILER} ${f_target_file}
    -o ${f_compiled_target_file} -L ${CMAKE_BINARY_DIR}
    -L ${CMAKE_BINARY_DIR}/external/basic -ltlm-basic -lsystemc_lib -lpthread VERBATIM)

  add_custom_command(OUTPUT ${f_staticopt_bc_target_file}
    DEPENDS ${f_target_file} pinavm
    COMMAND $<TARGET_FILE:pinavm> -b static -dis-dbg-msg ${f_target_file} VERBATIM)

  add_custom_command(OUTPUT ${f_staticopt_target_file}
    DEPENDS ${f_staticopt_bc_target_file} systemc_lib tlm-basic
    COMMAND ${LLVM_COMPILER} ${f_staticopt_bc_target_file}
    -o ${f_staticopt_target_file} -lsystemc_lib -ltlm-basic -L ${CMAKE_BINARY_DIR}
    -L ${CMAKE_BINARY_DIR}/external/basic -lpthread VERBATIM)

  add_custom_target(${f_target_name} DEPENDS ${f_target_file})
  add_custom_target(${f_target_name}.exe DEPENDS ${f_compiled_target_file})
  add_custom_target(${f_target_name}.opt DEPENDS ${f_staticopt_target_file})
endfunction(build_llvm_bitcode)

