find_program(LLVM_LINK "llvm-link-${LLVM_RECOMMAND_VERSION}"
		NAMES llvm-link
		HINTS ${LLVM_ROOT}/bin)

if(${LLVM_LINK} STREQUAL "LLVM_LINK-NOTFOUND")
  message(FATAL_ERROR "finds no LLVM_LINK")
endif()
