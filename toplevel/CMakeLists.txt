FILE(GLOB TOP_SRC *.cpp)
needs_no_rtti(toplevel.cpp)

Include_Directories(../frontend ../backends/TwetoBackend/ ../backends/42Backend/ ../backends/PromelaBackend/ ../backends/SimpleBackend/)
Add_Executable(pinavm ${TOP_SRC} $<TARGET_OBJECTS:systemc_lib>)

# Link to backends
target_link_libraries(pinavm tweto promela quarantedeux simple)

# Link to frontend and others
target_link_libraries(pinavm frontend utils tlm-basic)

# Link to LLVM libraries, pthread and dl
target_link_libraries(pinavm ${REQ_LLVM_LIBRARIES})
