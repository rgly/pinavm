macro(findLLVMTool var_name tool_name)
  find_program(${var_name} "${tool_name}-${LLVM_PATCH_VERSION}"
		NAMES "${tool_name}-${LLVM_RECOMMAND_VERSION}" ${tool_name}
		HINTS ${LLVM_ROOT}/bin)

  if(${${var_name}} STREQUAL "${var_name}-NOTFOUND")
    message(FATAL_ERROR "finds no ${var_name}")
  else()
    message(STATUS "${var_name} found : ${${var_name}}")
  endif()
endmacro(findLLVMTool)

findLLVMTool(LLVM_LINK llvm-link)
findLLVMTool(LLVM_OPT opt)
findLLVMTool(LLVM_COMPILER clang++)
SET(LLVMC_INCLUDE_DIR
               "-I${CMAKE_SOURCE_DIR}/external/systemc-${SYSTEMC_VERSION}/src/"
               "-I${CMAKE_SOURCE_DIR}/external/basic")
set(LLVMC_FLAGS -emit-llvm ${LLVM_DEFINITIONS} ${LLVMC_INCLUDE_DIR}
	-std=c++0x -fno-inline-functions -fno-use-cxa-atexit -c )
