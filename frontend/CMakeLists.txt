FILE(GLOB FRONTEND_CONSTRUCT_SRC SCConstructs/*.cpp)
FILE(GLOB FRONTEND_SRC *.cpp)
needs_no_rtti(FunctionBuilder.cpp)
needs_no_rtti(FrontendPass.cpp)

# include frontend itself in order to let SCConstructs can include here.
Include_directories(${CMAKE_CURRENT_SOURCE_DIR})

Add_Library(frontend ${FRONTEND_SRC} ${FRONTEND_CONSTRUCT_SRC})
