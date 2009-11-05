.SUFFIXES:

./configure: configure.ac
	autoconf

config.status: ./configure
	./configure

%: %.in config.status
	./config.status $*
