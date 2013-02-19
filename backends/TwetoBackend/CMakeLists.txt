FILE(GLOB TWETO_SRC *.cpp)

Include_Directories(../../frontend)
needs_no_rtti(TLMBasicPass)

Add_Library(tweto ${TWETO_SRC})
