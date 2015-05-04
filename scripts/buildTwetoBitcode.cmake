# Build LLVM bitcode and some testing executable for Tweto Backend
function(build_tweto_llvm_bitcode f_target_name f_src_list)
  build_llvm_bitcode(${f_target_name} ${f_src_list})

  set(f_target_file ${CMAKE_CURRENT_BINARY_DIR}/${f_target_name}.${LLVM_EXT})
  set(f_compiled_target_file ${CMAKE_CURRENT_BINARY_DIR}/${f_target_name}.exe)
  set(f_staticopt_bc_target_file ${CMAKE_CURRENT_BINARY_DIR}/output.${LLVM_EXT})
  set(f_staticopt_target_file ${CMAKE_CURRENT_BINARY_DIR}/${f_target_name}.opt)

  set(TWETO_LINK_FLAG -L ${CMAKE_BINARY_DIR} 
    -L ${CMAKE_BINARY_DIR}/external/basic
    -ltlm-basic -lsystemc_lib -lpthread)

  add_custom_command(OUTPUT ${f_compiled_target_file}
    DEPENDS ${f_target_file} systemc_lib tlm-basic
    COMMAND ${LLVM_COMPILER} ${f_target_file}
    -o ${f_compiled_target_file} ${TWETO_LINK_FLAG}
    VERBATIM)

  add_custom_command(OUTPUT ${f_staticopt_bc_target_file}
    DEPENDS ${f_target_file} pinavm
    COMMAND $<TARGET_FILE:pinavm> -b static -dis-dbg-msg ${f_target_file} VERBATIM)

  add_custom_command(OUTPUT ${f_staticopt_target_file}
    DEPENDS ${f_staticopt_bc_target_file} systemc_lib tlm-basic
    COMMAND ${LLVM_COMPILER} ${f_staticopt_bc_target_file}
    -o ${f_staticopt_target_file} ${TWETO_LINK_FLAG}
    VERBATIM)

  add_custom_target(${f_target_name}.exe DEPENDS ${f_compiled_target_file})
  add_custom_target(${f_target_name}.opt DEPENDS ${f_staticopt_target_file})
endfunction(build_tweto_llvm_bitcode)
