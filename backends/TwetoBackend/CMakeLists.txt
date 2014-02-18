FILE(GLOB TWETO_SRC *.cpp)

Include_Directories(${CMAKE_SOURCE_DIR}/frontend)
needs_no_rtti(TLMBasicPass)

Add_Library(tweto ${TWETO_SRC})
add_subdirectory(runtime_lib)
