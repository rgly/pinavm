INCLUDE=../../external/systemc-2.2.0/src/ 

hello.bc: hello.cpp
	llvm-g++ -I $(INCLUDE) $< -c --emit-llvm -o $@

.PHONY: run
run:
	../../toplevel/pinavm -b tweto hello.bc
