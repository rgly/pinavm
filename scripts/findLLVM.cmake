# Follows instructions from this site.
# http://llvm.org/docs/CMake.html#embedding-llvm-in-your-project

# the llvm version is setting to this variable.
SET(LLVM_RECOMMAND_VERSION 3.2)

if(NOT DEFINED ${LLVM_ROOT})
  # find llvm-config. perfers to the one with version suffix, Ex:llvm-config-3.2
  find_program(LLVM_CONFIG_EXE NAMES "llvm-config-${LLVM_RECOMMAND_VERSION}" "llvm-config")

  # Get the directory of llvm by using llvm-config. also remove whitespaces.
  execute_process(COMMAND ${LLVM_CONFIG_EXE} --prefix OUTPUT_VARIABLE LLVM_ROOT
		 OUTPUT_STRIP_TRAILING_WHITESPACE )
endif()

# We incorporate the CMake features provided by LLVM:
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${LLVM_ROOT}/share/llvm/cmake")

# Not using version check in find_package is that llvm version is 3.2svn,
# which is different to 3.2 of debian package pool.
# So checks version later.
find_package(LLVM REQUIRED)

# Check whether LLVM 3.2 is found.
if(NOT ${LLVM_FOUND})
  message(FATAL_ERROR "LLVM_ROOT(${LLVM_ROOT}) is not a valid LLVM install\n"
		"You can explicitly specify your llvm_root by "
		"cmake /where/pinavm/is -DLLVM_ROOT=/my/llvm/install/dir\n"
		"or make llvm-config visible in $PATH")
endif()

# Check whether the LLVM version meets our requirement.
# Maybe an ERROR is better than a WARNING message here?
if( NOT ("${LLVM_VERSION_MAJOR}.${LLVM_VERSION_MINOR}" STREQUAL ${LLVM_RECOMMAND_VERSION} ))
  message(WARNING "LLVM version is recommanded to be ${LLVM_RECOMMAND_VERSION}")
endif()

# Use settings from LLVM cmake module.
include_directories( ${LLVM_INCLUDE_DIRS} )
link_directories( ${LLVM_LIBRARY_DIRS} )
add_definitions( ${LLVM_DEFINITIONS} )

llvm_map_components_to_libraries(REQ_LLVM_LIBRARIES jit interpreter nativecodegen bitreader selectiondag asmparser linker)

# Find a compiler which compiles c++ source into llvm bitcode.
# It first finds clang, then it finds llvm-g++ if there is no clang. 
find_program(LLVM_COMPILER "clang++-${LLVM_RECOMMAND_VERSION}"
		NAMES clang++ clang llvm-g++ llvm-gcc
		HINTS ${LLVM_ROOT}/bin )

# Checks whether a LLVM_COMPILER is found, give a warning if not found.
# A warning instread of error is beceuse that we don't need clang during
# building pinavm.
if(${LLVM_COMPILER} STREQUAL "LLVM_COMPILER-NOTFOUND")
  message(FATAL_ERROR "Could not find clang or llvm-g++."
		" Please install one of them !")
else()
  message(STATUS "Use ${LLVM_COMPILER} as llvmc")
endif()

SET(LLVMC_INCLUDE_DIR "-I${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/src/"
                    "-I${CMAKE_SOURCE_DIR}/external/TLM-2009-07-15/include/tlm"
                    "-I${CMAKE_SOURCE_DIR}/external/basic")
SET(LLVMC_FLAGS ${LLVMC_INCLUDE_DIR} ${LLVM_DEFINITIONS} -fno-inline-functions
                -fno-use-cxa-atexit -emit-llvm -c )

# For debug use only
if(false)
  message("debug messages below")
  message("LLVM_ROOT is ${LLVM_ROOT}")
  message("REQ_LLVM_LIBRARIES is ${REQ_LLVM_LIBRARIES}")
  message("LLVM_COMPILER is ${LLVM_COMPILER}")
  message("LLVM_DEFINITIONS is ${LLVM_DEFINITIONS}")
  message("LLVM_LIBRARY_DIRS is ${LLVM_LIBRARY_DIRS}")
  message("LLVM_INCLUDE_DIRS is ${LLVM_INCLUDE_DIRS}")
endif()
