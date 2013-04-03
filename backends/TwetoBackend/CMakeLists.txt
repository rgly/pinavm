FILE(GLOB TWETO_SRC *.cpp)

Include_Directories(${CMAKE_SOURCE_DIR}/frontend)
needs_no_rtti(TLMBasicPass)

Add_Library(tweto ${TWETO_SRC})

# add some function symbols used by Tweto
SET(call_method_ll ${CMAKE_CURRENT_BINARY_DIR}/tweto_call_method.ll)
SET(call_method_source ${CMAKE_CURRENT_SOURCE_DIR}/runtime_lib/tweto_call_method.cpp)

add_custom_command(OUTPUT ${call_method_ll} DEPENDS ${call_method_source}
  COMMAND ${LLVM_COMPILER} ${LLVMC_FLAGS}
  ${call_method_source} -o ${call_method_ll} VERBATIM)

add_custom_target(tweto_call_method DEPENDS ${call_method_ll})
add_dependencies(tweto tweto_call_method)
