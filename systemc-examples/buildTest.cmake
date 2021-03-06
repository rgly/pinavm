
function(add_pinavm_test f_name f_backend_list f_source_list f_arg_list)
  if( (NOT DEFINED ${f_backend_list}) OR (NOT DEFINED ${f_source_list}) )
    message(FATAL_ERROR "calling empty list")
  endif()

  list(FIND ${f_backend_list} "tweto" tweto_backend_index)
  if(${tweto_backend_index} STREQUAL "-1")
    set(has_tweto_backend FALSE)
  else()
    set(has_tweto_backend TRUE)
  endif()

  if(NOT TARGET ${f_name})
    if(${has_tweto_backend})
      build_tweto_llvm_bitcode(${f_name} ${f_source_list})
      add_dependencies(gen_test_opt ${f_name}.opt)
      add_dependencies(gen_test_internal ${f_name}.exe)
    else(${has_tweto_backend})
      build_llvm_bitcode(${f_name} ${f_source_list})
    endif(${has_tweto_backend})

    add_dependencies(gen_test_source ${f_name})
  endif()

  foreach(temp_backend ${${f_backend_list}})
    add_test(NAME ${f_name}_${temp_backend}
            COMMAND $<TARGET_FILE:pinavm> -b ${temp_backend}
            -o ${CMAKE_CURRENT_BINARY_DIR}/${f_name}.${temp_backend}
            -dis-dbg-msg
            ${CMAKE_CURRENT_BINARY_DIR}/${f_name}.${LLVM_EXT} ${${f_arg_list}})
  endforeach(temp_backend)
endfunction(add_pinavm_test)

# pwd = /A/B/C/D, we get D for our test name.
macro(getLastDirectory f_out f_input) 
  STRING(REPLACE "/" ";" f_test_name_list ${f_input})
  LIST(LENGTH f_test_name_list f_list_size) 
  MATH(EXPR f_list_size ${f_list_size}-1)
  LIST(GET f_test_name_list ${f_list_size} ${f_out}) 
endmacro(getLastDirectory) 
