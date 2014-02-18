# add some function symbols used by Tweto

FILE(GLOB TWETO_extern_SRC *.cpp)
# it will generate tweto_call_method.bc
build_llvm_bitcode(tweto_call_method TWETO_extern_SRC)
add_dependencies(tweto tweto_call_method)
