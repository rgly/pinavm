CONFFILES=config.sh config.mk

all: toplevel

toplevel: configure-all
	cd toplevel/ && $(MAKE)

.SUFFIXES:

./configure: configure.ac
	autoconf -o $@ $<

config.status: ./configure
	if [ -x ./config.status ]; then \
		./config.status --recheck ;\
	else \
		echo "Please, run the configure script in $(PWD)" ;\
		echo "and try again." ;\
		exit 1 ;\
	fi

%: %.in config.status
	@./config.status $*

configure-all: $(CONFFILES)
