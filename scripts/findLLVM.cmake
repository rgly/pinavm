# Follows instructions from this site.
# http://llvm.org/docs/CMake.html#embedding-llvm-in-your-project

# the llvm version is setting to this variable.
SET(LLVM_RECOMMAND_VERSION 3.2)
# the llvm libraries we need.
SET(NEED_LLVM_LIB jit interpreter nativecodegen bitreader
			selectiondag asmparser linker)

# get the absolute path of LLVM_ROOT if it has a definition.
if(DEFINED LLVM_ROOT)
  get_filename_component(LLVM_ROOT ${LLVM_ROOT} ABSOLUTE)
  message(STATUS "use LLVM_ROOT : ${LLVM_ROOT}")
endif()

# find llvm-config. perfers to the one with version suffix, Ex:llvm-config-3.2
find_program(LLVM_CONFIG_EXE NAMES
      		"llvm-config-${LLVM_RECOMMAND_VERSION}" "llvm-config"
		PATHS ${LLVM_ROOT}/bin)

# In case of finds no LLVM, give user a hint to install LLVM
if(${LLVM_CONFIG_EXE} STREQUAL "LLVM_CONFIG_EXE-NOTFOUND")
  if(NOT DEFINED AUTOINSTALL)
    SET(AUTOINSTALL FALSE)
  endif()

  if(${AUTOINSTALL})
    # if AUTOINSTALL is explicitly set to true, then run installLLVM.
    include(${CMAKE_SOURCE_DIR}/scripts/installLLVM.cmake)
    # this find_program(llvm-config) should success.
    find_program(LLVM_CONFIG_EXE NAMES
      		"llvm-config-${LLVM_RECOMMAND_VERSION}" "llvm-config"
		PATHS ${LLVM_ROOT}/bin)
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

# after here. LLVM_CONFIG_EXE is found.
# we are going to set LLVM_ROOT
if(NOT DEFINED LLVM_ROOT)
  # Get the directory of llvm by using llvm-config. also remove whitespaces.
  execute_process(COMMAND ${LLVM_CONFIG_EXE} --prefix
		 OUTPUT_VARIABLE LLVM_ROOT
                 OUTPUT_STRIP_TRAILING_WHITESPACE )
endif()

# set the cmake prefix so that we can search thing the ${LLVM_ROOT}
LIST(APPEND CMAKE_PREFIX_PATH ${LLVM_ROOT}/bin)



# try to load the CMake module of LLVM.
include(${CMAKE_SOURCE_DIR}/scripts/loadLLVMModule.cmake)

# Check whether LLVM 3.2 is found.
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

# Check whether the LLVM version meets our requirement.
# Maybe an ERROR is better than a WARNING message here?
if( NOT (${LLVM_VERSION} MATCHES ${LLVM_RECOMMAND_VERSION}* ))
  message(WARNING "LLVM version is recommanded to be ${LLVM_RECOMMAND_VERSION}\n"
	"Your current version is ${LLVM_VERSION}")
endif()

# Use settings from LLVM cmake module or llvm-config.
include_directories( ${LLVM_INCLUDE_DIRS} )
link_directories( ${LLVM_LIBRARY_DIRS} )
add_definitions( ${LLVM_DEFINITIONS} )
SET(REQ_LLVM_LIBRARIES ${LLVM_LIBS})


# Find a compiler which compiles c++ source into llvm bitcode.
# ====================== find CLANG ===================================
find_program(CLANG "clang++-${LLVM_RECOMMAND_VERSION}"
      	  NAMES clang++ 
      	  HINTS ${LLVM_ROOT}/bin /usr/bin )

if(NOT (${CLANG} STREQUAL "CLANG-NOTFOUND"))
  SET(CLANG_FOUND TRUE)
  SET(CLANG_FLAGS -emit-llvm )
  message(STATUS "find ${CLANG}")
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
      	  HINTS ${LLVM_ROOT}/lib /usr/lib )
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
else()
  if(${CLANG_FOUND})
    message(STATUS "Emit llvm bitcode with Clang")
    SET(LLVM_COMPILER ${CLANG})
    SET(LLVMC_FLAGS ${CLANG_FLAGS})
  else()
    message(FATAL_ERROR "Could not find either clang or dragonegg."
		" Please install one of them !")
  endif()
endif()



SET(LLVMC_INCLUDE_DIR "-I${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/src/"
                    "-I${CMAKE_SOURCE_DIR}/external/TLM-2009-07-15/include/tlm"
                    "-I${CMAKE_SOURCE_DIR}/external/basic")

# Generate llvm human readable assembly instead of llvm bitcode.
# Quote from DragonEgg site:
 #        Adding -fplugin-arg-dragonegg-emit-ir or -flto causes LLVM IR
 #        to be output (you need to request assembler output, -S, rather
 #        than object code output, -c, since otherwise gcc will pass the
 #        LLVM IR to the system assembler, which will doubtless fail to
 #        assemble it):

SET(LLVMC_FLAGS ${LLVMC_FLAGS} ${LLVM_DEFINITIONS} ${LLVMC_INCLUDE_DIR}
		-fno-inline-functions -fno-use-cxa-atexit -S )

if(NOT DEFINED TEST_CMAKE)
  SET(TEST_CMAKE FALSE)
endif()

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
