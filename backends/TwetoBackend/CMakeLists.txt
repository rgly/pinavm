FILE(GLOB TWETO_SRC *.cpp)

Include_Directories(../../frontend)
needs_no_rtti(TLMBasicPass)

Add_Library(tweto ${TWETO_SRC})

# add some function symbols used by Tweto
SET(call_method_bc ${CMAKE_CURRENT_BINARY_DIR}/tweto_call_method.bc)
SET(call_method_source ${CMAKE_CURRENT_SOURCE_DIR}/runtime_lib/tweto_call_method.cpp)

add_custom_command(OUTPUT ${call_method_bc} DEPENDS ${call_method_source}
  COMMAND ${LLVM_COMPILER} ${LLVMC_FLAGS}
  ${call_method_source} -o ${call_method_bc} VERBATIM)

add_custom_target(tweto_call_method DEPENDS ${call_method_bc})
add_dependencies(tweto tweto_call_method)
