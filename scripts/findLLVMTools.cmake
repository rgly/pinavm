macro(findLLVMTool var_name tool_name)
  find_program(${var_name} "${tool_name}-${LLVM_RECOMMAND_VERSION}"
		NAMES ${tool_name}
		HINTS ${LLVM_ROOT}/bin)

  if(${${var_name}} STREQUAL "${var_name}-NOTFOUND")
    message(FATAL_ERROR "finds no ${var_name}")
  else()
    message(STATUS "${var_name} found : ${${var_name}}")
  endif()
endmacro(findLLVMTool)


findLLVMTool(LLVM_LINK llvm-link)
findLLVMTool(LLVM_OPT opt)
