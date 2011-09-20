FILE(GLOB TOP_SRC *.cpp)

Include_Directories(../frontend ../backends/TwetoBackend/ ../backends/42Backend/ ../backends/PromelaBackend/ ../backends/SimpleBackend/)
Add_Executable(pinavm ${TOP_SRC})
add_dependencies(pinavm systemc-all)
target_link_libraries(pinavm frontend utils tweto promela simple quarantedeux tlm-basic)
target_link_libraries(pinavm ${LLVM_LIBS})
