FILE(GLOB FRONTEND_SRC *.cpp)
needs_no_rtti(FunctionBuilder.cpp)
needs_no_rtti(FrontendPass.cpp)

Add_Library(frontend ${FRONTEND_SRC})
