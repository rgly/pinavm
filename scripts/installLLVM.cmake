# This script downloads, configure, build and install LLVM for you,
# and those actions are running during cmake time, so the drawback
# is that users may find it takes a long time to finish this cmake
# script.

if(NOT DEFINED LLVM_RECOMMAND_VERSION)
  message(FATAL_ERROR "finds no LLVM_RECOMMAND_VERSION")
endif()

# give TEST_CMAKE a definition
if(NOT DEFINED TEST_CMAKE)
  SET(TEST_CMAKE FALSE)
endif()

if(${TEST_CMAKE})
  # for test purpose, I don't want to waste bandwidth of llvm.org.
  SET(SITE_URL file://${CMAKE_SOURCE_DIR}/../source-tgz/llvm32)
else()
  # normal situation.
  SET(SITE_URL http://llvm.org/releases/${LLVM_RECOMMAND_VERSION})
endif()

SET(DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/download )
SET(RECOMMAND_LLVM_PREFIX ${CMAKE_BINARY_DIR}/lib/llvm-${LLVM_RECOMMAND_VERSION})
SET(LLVM_SUFFIX "-${LLVM_RECOMMAND_VERSION}.src")

# just hard coding them.
SET(LLVM_NAME llvm)
SET(LLVM_FILE ${DOWNLOAD_DIR}/llvm${LLVM_SUFFIX}.tar.gz)
SET(LLVM_SOURCE_DIR ${CMAKE_BINARY_DIR}/llvm-source)
SET(LLVM_URL ${SITE_URL}/llvm${LLVM_SUFFIX}.tar.gz)
SET(LLVM_MD5 71610289bbc819e3e15fdd562809a2d7 ) 

SET(CLANG_NAME clang)
SET(CLANG_FILE ${DOWNLOAD_DIR}/clang${LLVM_SUFFIX}.tar.gz)
SET(CLANG_SOURCE_DIR ${LLVM_SOURCE_DIR}/llvm/tools)
SET(CLANG_URL ${SITE_URL}/clang${LLVM_SUFFIX}.tar.gz)
SET(CLANG_MD5 3896ef4334df08563b05d0848ba80582)

SET(COMPILER-RT_NAME compiler-rt)
SET(COMPILER-RT_FILE ${DOWNLOAD_DIR}/compiler-rt${LLVM_SUFFIX}.tar.gz)
SET(COMPILER-RT_SOURCE_DIR ${LLVM_SOURCE_DIR}/llvm/projects)
SET(COMPILER-RT_URL ${SITE_URL}/compiler-rt${LLVM_SUFFIX}.tar.gz)
SET(COMPILER-RT_MD5 a9a30ccd7bbee6f68a3ca3020af0d852)

Function(check_and_download target_file target_url target_md5)
  if((${target_file} STREQUAL "") OR 
     (${target_url} STREQUAL "") OR
     (${target_md5} STREQUAL "") )
    message(FATAL_ERROR "calling undefined variable."
             " there is a bug in installLLVM.cmake")
  endif()

  # if file not exists, download it.
  if(NOT EXISTS ${target_file})
    message(STATUS "finds no ${target_file}, so download it!")
    FILE(DOWNLOAD ${target_url} ${target_file} SHOW_PROGRESS
         EXPECTED_MD5 ${target_md5})
  else()
    # if exist, then check integrity.
    FILE(MD5 ${target_file} checksum)
    # if checksum is inequal, re-download it.
    if(NOT (${checksum} STREQUAL ${target_md5}))
      message(WARNING "error checksum for ${target_file}, so download it!")
      FILE(DOWNLOAD ${target_url} ${target_file} SHOW_PROGRESS
           EXPECTED_MD5 ${target_md5})
    endif()
  endif()
endfunction()

function(extract_file target_name target_file target_dir)
  if((${target_name} STREQUAL "") OR 
     (${target_file} STREQUAL "") OR
     (${target_dir} STREQUAL "") )
    message(FATAL_ERROR "calling undefined variable."
             " there is a bug in installLLVM.cmake")
  endif()

  # if finds no directory, then create one.
  if(NOT EXISTS ${target_dir})
    FILE(MAKE_DIRECTORY ${target_dir})
  endif()

  # Extract the archive. Note that the tar command is from cmake, so
  # it is portable.
  if(EXISTS ${target_file})
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar zxf ${target_file} 
			WORKING_DIRECTORY ${target_dir})
  else()
    message(FATAL_ERROR "finds no ${target_file}")
  endif()

  # the source code are in directory likes clang-3.2.src,
  # but cmake of llvm only knows clang. Rename it.
  FILE(RENAME ${target_dir}/${target_name}${LLVM_SUFFIX}
		 ${target_dir}/${target_name})
endfunction()




# Check whether llvm-config is installed.
# Especially ${LLVM_ROOT}, because we are going to install llvm on there.
find_program(llvm-config-temp
	NAMES "llvm-config-${LLVM_RECOMMAND_VERSION}" "llvm-config"
	PATHS ${LLVM_ROOT}/bin )

if (NOT (${llvm-config-temp} STREQUAL "llvm-config-temp-NOTFOUND"))
  message(FATAL_ERROR "The purpose this script is automatically install LLVM,"
		" It seems you already have LLVM."
		" This is a bug. Please contact authors.")
endif()

# download llvm, clang and compiler-rt here.
check_and_download(${LLVM_FILE} ${LLVM_URL} ${LLVM_MD5})
check_and_download(${CLANG_FILE} ${CLANG_URL} ${CLANG_MD5})
check_and_download(${COMPILER-RT_FILE} ${COMPILER-RT_URL} ${COMPILER-RT_MD5})

message(STATUS "finish llvm source download.")


# extract the source code.
extract_file(${LLVM_NAME} ${LLVM_FILE} ${LLVM_SOURCE_DIR})
extract_file(${CLANG_NAME} ${CLANG_FILE} ${CLANG_SOURCE_DIR})
extract_file(${COMPILER-RT_NAME} ${COMPILER-RT_FILE} ${COMPILER-RT_SOURCE_DIR})

message(STATUS "finish extraction for the source code.")

# Make sure there is a clear definition of LLVM_ROOT.
# because we are going to configure the llvm with LLVM_ROOT as prefix.
if(NOT DEFINED LLVM_ROOT)
  SET(LLVM_ROOT ${RECOMMAND_LLVM_PREFIX})
endif()

# Create necessary directories.
FILE(MAKE_DIRECTORY ${LLVM_ROOT})
FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/build-llvm)

# This script configures llvm for you.
execute_process(COMMAND ${CMAKE_COMMAND} ${LLVM_SOURCE_DIR}/llvm
		-DCMAKE_INSTALL_PREFIX=${LLVM_ROOT}
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/build-llvm)

message(STATUS "finish configure the source code.")
message(STATUS "Start to build LLVM, it may take tens of minutes."
		" If you worry about whether this script is still running,"
		" you can use \"top\" to monitor CPU usage.")

# it compiles with 4 threads. In hope of shortening compile time.
execute_process(COMMAND make -j4
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/build-llvm)

message(STATUS "we are going to install llvm and clang.")
execute_process(COMMAND make install
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/build-llvm)

# check whether llvm-config is installed again.
find_program(llvm-config-temp
	NAMES "llvm-config-${LLVM_RECOMMAND_VERSION}" "llvm-config"
	PATHS ${LLVM_ROOT}/bin)

# llvm should exists in user's system.
# if result is notfound, which means that install script failed, give an error.
if (${llvm-config-temp} STREQUAL "llvm-config-temp-NOTFOUND")
  message(FATAL_ERROR "You can run this script if you installed LLVM,"
		" This is a bug. Please contact developers.")
else()
  message(STATUS "Finish installing LLVM.")
endif()

# Currently we install llvm while cmake time, because we want to 
# have llvm before configuring pinavm. The following suggestion
# does not meet our requirement due to it installs llvm during build
# time.

# There is another way to implement autoinstall, but we need clang in
# llvm/tools directory before build. I am not sure how to do that.
#ExternalProject_Add( LLVM-3.2 URL ${LLVM_URL} URL_MD5 ${LLVM_MD5}
