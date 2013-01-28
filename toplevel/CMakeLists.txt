FILE(GLOB TOP_SRC *.cpp)

Include_Directories(../frontend ../backends/TwetoBackend/ ../backends/42Backend/ ../backends/PromelaBackend/ ../backends/SimpleBackend/)
Add_Executable(pinavm ${TOP_SRC} $<TARGET_OBJECTS:systemc_lib>)
target_link_libraries(pinavm frontend utils tweto promela simple quarantedeux tlm-basic)
target_link_libraries(pinavm ${LLVM_LIBS})


# llvm libraries might have dependeny orders.
# since g++-4.5, the default of recursive search was off.
# http://stackoverflow.com/questions/8725572/gcc-4-5-vs-4-4-linking-with-dependencies
target_link_libraries(pinavm pthread dl)

