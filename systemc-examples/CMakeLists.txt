SET(LLVM_EXT bc)
include(buildTest.cmake)
SET(TEST_BACKENDS promela hello)
SET(TWETO_BACKENDS tweto)

# We need to generate llvm assembly before running ctest. Create a cmake
# target so that we can generate all input files with "make gen_test_source"
add_custom_target(gen_test_source)
add_dependencies(pinavm gen_test_source)

# collect all the directory containing CMakeLists.txt
FILE(GLOB TEST_DIR */CMakeLists.txt)
foreach(sub_cmakelist ${TEST_DIR})
  get_filename_component(temp_dir ${sub_cmakelist} PATH)
  add_subdirectory(${temp_dir})
endforeach(sub_cmakelist)
