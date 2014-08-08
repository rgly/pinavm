include(${CMAKE_SOURCE_DIR}/scripts/installLLVM.cmake)

# find llvm-config. perfers to the one with version suffix, Ex:llvm-config-3.2
find_program(LLVM_CONFIG_EXE
  NAMES "${LLVM_ROOT}/bin/llvm-config-${LLVM_RECOMMAND_VERSION}"
        "${LLVM_ROOT}/bin/llvm-config"
        "llvm-config-${LLVM_RECOMMAND_VERSION}"
        "llvm-config"
	)

# In case of finds no LLVM, give user a hint to install LLVM
if(${LLVM_CONFIG_EXE} STREQUAL "LLVM_CONFIG_EXE-NOTFOUND")
  if(${AUTOINSTALL})
    # if AUTOINSTALL is explicitly set to true, then run installLLVM.
    autoinstall_llvm()
  else()
    # on condition that finds no LLVM and user not specify AUTOINSTALL.
    message(FATAL_ERROR "\tfinds no LLVM in your system.\n"
      	"\tPlease manually install LLVM.\n"
      	"\tOr (with root permission) :\n"
      	"\t\"cmake /where/pinavm -DAUTOINSTALL=TRUE\"\n"
      	"\twhich should automatically install LLVM for you"
      	" during cmake time.")
  endif()
endif()

message(STATUS "find LLVM-Config : ${LLVM_CONFIG_EXE}")
execute_process(COMMAND ${LLVM_CONFIG_EXE} --version
		OUTPUT_VARIABLE LLVM_VERSION
		OUTPUT_STRIP_TRAILING_WHITESPACE)


# Check whether the LLVM version meets our requirement.
if(  (${LLVM_VERSION} MATCHES ${LLVM_RECOMMAND_VERSION} )
  OR (${LLVM_VERSION} MATCHES ${LLVM_RECOMMAND_VERSION}svn))
  message(STATUS "LLVM version : ${LLVM_VERSION}")
else()
  if(${AUTOINSTALL})
    # if AUTOINSTALL is explicitly set to true, then run installLLVM.
    autoinstall_llvm()
  else()
    message(FATAL_ERROR "LLVM version is recommanded to be : "
         "${LLVM_RECOMMAND_VERSION}\n"
         "Your current version is ${LLVM_VERSION}")
  endif()
endif()

# In here. LLVM_CONFIG_EXE is found. We can get valid LLVM_ROOT.
execute_process(COMMAND ${LLVM_CONFIG_EXE} --prefix
		OUTPUT_VARIABLE LLVM_ROOT
                OUTPUT_STRIP_TRAILING_WHITESPACE )

# set the cmake prefix so that we can search thing the ${LLVM_ROOT}
LIST(APPEND CMAKE_PREFIX_PATH ${LLVM_ROOT}/bin)



# try to load the CMake module of LLVM.
include(${CMAKE_SOURCE_DIR}/scripts/loadLLVMModule.cmake)

# Check whether LLVM package is found.
if(NOT ${LLVM_FOUND})
  # if CMake module of LLVM is not found, we collect infomation
  # through llvm-config.
  include(${CMAKE_SOURCE_DIR}/scripts/loadLLVMConfig.cmake)
  if(NOT ${LLVM_FOUND})
    message(FATAL_ERROR "(${LLVM_ROOT}) is not a valid LLVM install\n"
		"You can explicitly specify your llvm_root by\n"
		"\"cmake /where/pinavm/is -DLLVM_ROOT=/my/llvm/install/dir\"\n"
		"or make llvm-config visible in $PATH")
  endif()
endif()

# Use settings from LLVM cmake module or llvm-config.
include_directories( ${LLVM_INCLUDE_DIRS} )
link_directories( ${LLVM_LIBRARY_DIRS} )
add_definitions( ${LLVM_DEFINITIONS} )
# probably some llvm-config work to do...
SET(REQ_LLVM_LIBRARIES ${LLVM_LIBS};curses;LLVMBitWriter)

# This script sets LLVMC_FLAGS and LLVM_COMPILER.
include(${CMAKE_SOURCE_DIR}/scripts/findLLVMC.cmake)
include(${CMAKE_SOURCE_DIR}/scripts/findLLVMTools.cmake)



# For debug use only
if(${TEST_CMAKE})
  message("debug messages below")
  message("LLVM_ROOT is ${LLVM_ROOT}")
  message("REQ_LLVM_LIBRARIES is ${REQ_LLVM_LIBRARIES}")
  message("LLVM_COMPILER is ${LLVM_COMPILER}")
  message("LLVMC_FLAGS is ${LLVMC_FLAGS}")
  message("LLVM_DEFINITIONS is ${LLVM_DEFINITIONS}")
  message("LLVM_LIBRARY_DIRS is ${LLVM_LIBRARY_DIRS}")
  message("LLVM_INCLUDE_DIRS is ${LLVM_INCLUDE_DIRS}")
endif()
