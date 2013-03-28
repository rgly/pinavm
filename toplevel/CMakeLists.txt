
# =================== Configure launch_backends.cpp ==========================
foreach(backend ${BACKENDS} Run) # Run is a special backend by Tweto
  string(TOLOWER "${backend}" lbackend)
  list(APPEND PRINT_BACKEND_INCLUDE "EXTERN_F(${lbackend})")
  list(APPEND PRINT_BACKEND_LAUNCH "LAUNCHBACKEND(${backend},${lbackend})")
  # Tweto also accepts backend names of "Run"
endforeach(backend)

string(REPLACE ";" "\n" PRINT_BACKEND_INCLUDE "${PRINT_BACKEND_INCLUDE}")
string(REPLACE ";" "\n" PRINT_BACKEND_LAUNCH "${PRINT_BACKEND_LAUNCH}")
configure_file(${CMAKE_SOURCE_DIR}/toplevel/launch_backends.cpp.in
               ${CMAKE_BINARY_DIR}/toplevel/launch_backends.cpp)


# ====================== Collect Source ==============================
FILE(GLOB TOP_SRC *.cpp)
needs_no_rtti(toplevel.cpp)

# ======================== Include ==============================
Include_Directories(${CMAKE_SOURCE_DIR}/frontend ${CMAKE_BINARY_DIR}/toplevel)
# include backend directories.
#foreach(backend ${BACKENDS})
#  Include_Directories(${CMAKE_SOURCE_DIR}/backends/${backend}Backend)
#endforeach(backend)

Add_Executable(pinavm ${TOP_SRC} $<TARGET_OBJECTS:systemc_lib>
		${CMAKE_BINARY_DIR}/toplevel/launch_backends.cpp)

# ======================== Link Backends ==============================
# lower letters 
string(TOLOWER "${BACKENDS}" backends_lib)
# 42backend is an exception, it use quarantedeux as target name.
string(REPLACE "42" "quarantedeux" backends_lib "${backends_lib}")
# Link to backends
foreach(backend ${backends_lib})
  target_link_libraries(pinavm ${backend})
endforeach(backend)

# ====================== Link frontend & llvm ==========================
target_link_libraries(pinavm frontend utils tlm-basic)
# Link to LLVM libraries, pthread and dl
target_link_libraries(pinavm ${REQ_LLVM_LIBRARIES})



