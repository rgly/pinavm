CONFFILES=config.sh config.mk

all: configure-all

.SUFFIXES:

./configure: configure.ac
	autoconf -o $@ $<

config.status: ./configure
	./config.status --recheck

%: %.in config.status
	@./config.status $*

configure-all: $(CONFFILES)
