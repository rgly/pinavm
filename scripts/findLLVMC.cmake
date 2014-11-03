# Find a compiler which compiles c++ source into llvm assembly.
# ====================== find CLANG ===================================
find_program(CLANG "clang++-${LLVM_PATCH_VERSION}"
      	  NAMES "clang++-${LLVM_RECOMMAND_VERSION}" clang++ 
      	  HINTS ${LLVM_ROOT}/bin)

if(NOT (${CLANG} STREQUAL "CLANG-NOTFOUND"))
  SET(CLANG_FOUND TRUE)
  SET(CLANG_FLAGS -emit-llvm )
  message(STATUS "Clang found : ${CLANG}")
else()
  SET(CLANG_FOUND FALSE)
endif()

# ===================== find DragonEgg =================================
# Since DragonEgg is compiled with certain Gcc version, and I do not
# have any good idea to detect the gcc version which DragonEgg use.
# Users should explicitly specify gcc version if they want to use
# DragonEgg.  If users do not specify gcc version, then it is
# meaningless to find DragonEgg.
if(DEFINED DRAGONEGG_GCC)
  # set it to true before finding anything.
  SET(DRAGONEGG_FOUND TRUE)

  # find the dragonegg, the gcc plugin.
  find_library(DRAGONEGG "dragonegg-${LLVM_RECOMMAND_VERSION}.so"
      	  NAMES dragonegg.so
      	  HINTS ${LLVM_ROOT}/lib)
  if(${DRAGONEGG} STREQUAL "DRAGONEGG-NOTFOUND")
    SET(DRAGONEGG_FOUND FALSE)
    message(WARNING "Finds no dragonegg.so")
  endif()

  # User may specify DRAGONEGG_GCC=g++-4.6, but I need absolute path.
  find_program(ABS_DRAGONEGG_GCC ${DRAGONEGG_GCC})
  if(${ABS_DRAGONEGG_GCC} STREQUAL "ABS_DRAGONEGG_GCC-NOFOUND")
    SET(DRAGONEGG_FOUND FALSE)
    message(WARNING "Finds no DRAGONEGG_GCC = \"${DRAGONEGG_GCC}\"")
  endif()

  # if condition true, means CMake finds the dragonegg.so and the correct gcc.
  if(${DRAGONEGG_FOUND})
    SET(DRAGONEGG_FOUND TRUE)
    message(STATUS "find ${DRAGONEGG} with ${ABS_DRAGONEGG_GCC}")
    SET(DRAGONEGG_FLAGS -fplugin=${DRAGONEGG} -fplugin-arg-dragonegg-emit-ir)
  endif()
endif()

# I guess that users perfer DragonEgg if they are willing to take
# a few seconds to specify DRAGONEGG_GCC,
# If users specify nothing(ex: -DDRAGONEGG_GCC=g++-4.6), CMake would not
# find DragonEgg, thus Clang is the only candidate.
if(${DRAGONEGG_FOUND})
  message(STATUS "Emit llvm with DragonEgg")
  SET(LLVM_COMPILER ${ABS_DRAGONEGG_GCC})
  SET(LLVMC_FLAGS ${DRAGONEGG_FLAGS})
else(${DRAGONEGG_FOUND})
  if(${CLANG_FOUND})
    message(STATUS "Emit LLVM bitcode with Clang")
    SET(LLVM_COMPILER ${CLANG})
    SET(LLVMC_FLAGS ${CLANG_FLAGS})
  else(${CLANG_FOUND})
    message(FATAL_ERROR "Could not find either clang or dragonegg."
		" Please install one of them !")
  endif(${CLANG_FOUND})
endif(${DRAGONEGG_FOUND})

SET(LLVMC_INCLUDE_DIR
               "-I${CMAKE_SOURCE_DIR}/external/systemc-${SYSTEMC_VERSION}/src/"
               "-I${CMAKE_SOURCE_DIR}/external/basic")

# Generate llvm human readable assembly instead of llvm bitcode.
# Quote from DragonEgg site:
 #        Adding -fplugin-arg-dragonegg-emit-ir or -flto causes LLVM IR
 #        to be output (you need to request assembler output, -S, rather
 #        than object code output, -c, since otherwise gcc will pass the
 #        LLVM IR to the system assembler, which will doubtless fail to
 #        assemble it):

LIST(APPEND LLVMC_FLAGS ${LLVM_DEFINITIONS} ${LLVMC_INCLUDE_DIR}
	-std=c++0x -fno-inline-functions -fno-use-cxa-atexit -S )
