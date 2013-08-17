# This script downloads, configure, build and install LLVM for you,
# and those actions are running during cmake time, so the drawback
# is that users may find it takes a long time to finish this cmake
# script.

# NOTE that this script requires tar and make command in your OS.

MACRO(configure_autoinstall)
  if(NOT DEFINED LLVM_RECOMMAND_VERSION)
    message(FATAL_ERROR "finds no LLVM_RECOMMAND_VERSION")
  endif()

  if(${TEST_CMAKE})
    # for test purpose, I don't want to waste bandwidth of llvm.org.
    SET(SITE_URL
       file://${CMAKE_SOURCE_DIR}/../source-tgz/llvm-${LLVM_RECOMMAND_VERSION})
  else()
    # normal situation.
    SET(SITE_URL http://llvm.org/releases/${LLVM_RECOMMAND_VERSION})
  endif()

  SET(DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/download )
  SET(LLVM_SUFFIX "-${LLVM_RECOMMAND_VERSION}.src")

  SET(LLVM_NAME llvm)
  SET(LLVM_SOURCE_DIR ${CMAKE_BINARY_DIR}/llvm-source/${LLVM_NAME})

  # Clang uses cfe(C frontend) as its file name from version 3.3. In
  # order to make cmake of llvm automatically find and compile Clang,
  # a tools/clang directory is still needed.
  if (${LLVM_RECOMMAND_VERSION} VERSION_LESS "3.3")
    SET(CLANG_NAME clang)
  else()
    SET(CLANG_NAME cfe)
  endif()
  SET(CLANG_SOURCE_DIR ${LLVM_SOURCE_DIR}/tools/clang)

  SET(COMPILER-RT_NAME compiler-rt)
  SET(COMPILER-RT_SOURCE_DIR ${LLVM_SOURCE_DIR}/projects/${COMPILER-RT_NAME})

  if (${LLVM_RECOMMAND_VERSION} STREQUAL "3.2")
    SET(LLVM_MD5 71610289bbc819e3e15fdd562809a2d7) 
    SET(CLANG_MD5 3896ef4334df08563b05d0848ba80582)
    SET(COMPILER-RT_MD5 a9a30ccd7bbee6f68a3ca3020af0d852)
  endif()

  if (${LLVM_RECOMMAND_VERSION} STREQUAL "3.3")
    SET(LLVM_MD5 40564e1dc390f9844f1711c08b08e391) 
    SET(CLANG_MD5 8284891e3e311829b8e44ac813d0c9ef)
    SET(COMPILER-RT_MD5 9c129ce24514467cfe492cf2fed8e2c4)
  endif()
endmacro()

FUNCTION(check_and_download target_name target_md5)
  if("${target_md5}" STREQUAL "" )
    message(FATAL_ERROR "calling undefined variable."
             " there is a bug in installLLVM.cmake")
  endif()

  SET(target_file ${DOWNLOAD_DIR}/${target_name}${LLVM_SUFFIX}.tar.gz)
  SET(target_url ${SITE_URL}/${target_name}${LLVM_SUFFIX}.tar.gz)

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

FUNCTION(extract_file target_name target_dir)
  if(("${target_name}" STREQUAL "") OR 
     ("${target_dir}" STREQUAL "") )
    message(FATAL_ERROR "calling undefined variable."
             " there is a bug in installLLVM.cmake")
  endif()

  SET(target_file ${DOWNLOAD_DIR}/${target_name}${LLVM_SUFFIX}.tar.gz)

  # if finds no directory, then create one.
  if(NOT EXISTS ${target_dir})
    FILE(MAKE_DIRECTORY ${target_dir})
  endif()

  # Extract the archive. Note that CMake-built-in tar does not
  # support --strip-components.
  if(EXISTS ${target_file})
    execute_process(COMMAND tar zxf ${target_file} --strip-components=1 
			WORKING_DIRECTORY ${target_dir})
  else()
    message(FATAL_ERROR "finds no ${target_file}")
  endif()


endfunction()

FUNCTION(install_llvm)
  SET(llvm_build_dir ${CMAKE_BINARY_DIR}/build-llvm)
  # Create necessary directories.
  if (NOT EXISTS ${LLVM_ROOT})
    FILE(MAKE_DIRECTORY ${LLVM_ROOT})
  endif()

  if (NOT EXISTS ${llvm_build_dir})
    FILE(MAKE_DIRECTORY ${llvm_build_dir})
  endif()

  # This script configures llvm for you.
  execute_process(COMMAND ${CMAKE_COMMAND} ${LLVM_SOURCE_DIR}
		  -DCMAKE_INSTALL_PREFIX=${LLVM_ROOT}
		  WORKING_DIRECTORY ${llvm_build_dir})

  message(STATUS "finish configure the source code.")
  message(STATUS "Start to build and install LLVM, it may take tens of minutes."
		" If you worry about whether this script is still running,"
		" you can use \"top\" to monitor CPU usage.")

  # it compiles with 4 threads. In hope of shortening compile time.
  execute_process(COMMAND make -j4
		  WORKING_DIRECTORY ${llvm_build_dir})

  execute_process(COMMAND make install
		  WORKING_DIRECTORY ${llvm_build_dir})

endfunction()

FUNCTION(autoinstall_llvm)
  configure_autoinstall()

  # download llvm, clang and compiler-rt here.
  check_and_download(${LLVM_NAME} ${LLVM_MD5})
  check_and_download(${CLANG_NAME} ${CLANG_MD5})
  check_and_download(${COMPILER-RT_NAME} ${COMPILER-RT_MD5})

  message(STATUS "finish llvm source download.")


  # extract the source code. keep 2nd arg null means same with 1st arg.
  extract_file(${LLVM_NAME} ${LLVM_SOURCE_DIR})
  extract_file(${CLANG_NAME} ${CLANG_SOURCE_DIR})
  extract_file(${COMPILER-RT_NAME} ${COMPILER-RT_SOURCE_DIR})

  message(STATUS "finish extraction for the source code.")

  # temperary commement out this for test usage.
  install_llvm()

  # check whether llvm-config is installed again.
  find_program(llvm-config-temp
	  NAMES "llvm-config-${LLVM_RECOMMAND_VERSION}" "llvm-config"
	  HINTS ${LLVM_ROOT}/bin)

  # llvm should exists in user's system.
  # if notfound, which means that install script failed, give an error.
  if (${llvm-config-temp} STREQUAL "llvm-config-temp-NOTFOUND")
    message(FATAL_ERROR "This is a bug. Please contact developers.")
  else()
    message(STATUS "Finished installing LLVM.")
    # set LLVM_CONFIG_EXE for parent, so that not to find it in parent
    # process again.
    set(LLVM_CONFIG_EXE ${llvm-config-temp} PARENT_SCOPE)
  endif()
endfunction()


# Currently we install llvm while cmake time, because we want to 
# have llvm before configuring pinavm. The following suggestion
# does not meet our requirement due to it installs llvm during build
# time.

# There is another way to implement autoinstall, but we need clang in
# llvm/tools directory before build. I am not sure how to do that.
#ExternalProject_Add( LLVM-${LLVM_RECOMMAND_VERSION} URL ${LLVM_URL} URL_MD5 ${LLVM_MD5}
