testdlopen: testdlopen.cpp
	g++ -g $< -o $@ -ldl

testdlopen-rdynamic: testdlopen.cpp
	g++ -g $< -o $@ -ldl -rdynamic

testdlopen-dynamic: testdlopen.cpp libtestdlopen.so
	g++ -g $< -o $@ -ldl -ltestdlopen -L.

libtestdlopen.so: testdlopen.cpp
	g++ -shared -g $< -o $@

run: testdlopen libtestdlopen.so testdlopen-dynamic testdlopen-rdynamic
# this one can't access it's own symbols.
	-./testdlopen
	@echo
# this one is able to access the symbols, through the shared library.
	-LD_LIBRARY_PATH=. ./testdlopen-dynamic
	@echo
	././testdlopen-rdynamic
