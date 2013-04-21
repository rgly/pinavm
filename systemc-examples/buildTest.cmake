function(build_llvm_assembly f_target_name f_src_list)
  set(f_target_file ${CMAKE_CURRENT_BINARY_DIR}/${f_target_name}.${LLVM_EXT})

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

  add_custom_target(${f_target_name} DEPENDS ${f_target_file})
  add_dependencies(gen_test_source ${f_target_name})
endfunction(build_llvm_assembly)


function(add_pinavm_test f_name f_backend_list f_source_list f_arg_list)
  if( (NOT DEFINED ${f_backend_list}) OR (NOT DEFINED ${f_source_list}) )
    message(FATAL_ERROR "calling empty list")
  endif()

  if(NOT TARGET ${f_name})
    build_llvm_assembly(${f_name} ${f_source_list})
  endif()

  foreach(temp_backend ${${f_backend_list}})
    add_test(NAME ${f_name}_${temp_backend}
            COMMAND $<TARGET_FILE:pinavm> -b ${temp_backend}
            -o ${CMAKE_CURRENT_BINARY_DIR}/${f_name}.${temp_backend}
            -dis-dbg-msg
            ${f_name}.${LLVM_EXT} ${${f_arg_list}})
  endforeach(temp_backend)
endfunction(add_pinavm_test)

# pwd = /A/B/C/D, we get D for our test name.
macro(getLastDirectory f_out f_input) 
  STRING(REPLACE "/" ";" f_test_name_list ${f_input})
  LIST(LENGTH f_test_name_list f_list_size) 
  MATH(EXPR f_list_size ${f_list_size}-1)
  LIST(GET f_test_name_list ${f_list_size} ${f_out}) 
endmacro(getLastDirectory) 
