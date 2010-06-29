

OVERRIDING = default

#Default Timeout Value 2 seconds of CPU TIME (can be overriden by command line arguments)
TIMEOUT = 3

#Default Argument Value 3 (can be overriden by command line arguments)
ARG=3

SUB_DIRS = arth_ctrl \
events  \
elab_easy  \
elab_easy_arrays \
elab_easy_int \
elab_easy_sc_stop \
elab_easy_uint \
elab_multiple_instances \
elab_ports \
elab_ports_array \
elab_ports_bool \
elab_ports_pointer \
empty-sc \
fifo \
jerome-chain \
jerome-chain-deterministic \
pkt_switch \
RAM \
sc_clock signal


PASS = Pass
FAIL = Fail
OUTPUT = RESULT
ALLRESULTS = Results_all
SMRESULTS= Simple_Results
FERESULTS= Frontend_Results
PRRESULTS= Promela_Results
SPINRESULTS= Spin_Results
SPIN = spin

.PHONY: test clean promela simple all frontend

#simu:

# Do make all (optional)TIMEOUT=.. (optional)ARG=.. and see the generated results file Frontend_Results Simple_Results and Promela_Results

all:	frontend promela simple spin
#	$(MAKE) frontend
#	$(MAKE)	promela
#	$(MAKE) simple 

# Do make frontend (optional)TIMEOUT=.. (optional)ARG=.. and see the generated result file Frontend_Results
frontend:
	echo -e "TESTING WITH Frontend ##################\n">$(FERESULTS)
	for d in $(SUB_DIRS); 								\
	do                                                                      	\
	    echo ......................................................; 		\
	    echo Testing $$d with Frontend; 						\
	    echo running with $(ARG) and $(OVERRIDING); 				\
	    cd $$d; 									\
	    ulimit -t $(TIMEOUT);							\
	    if $(MAKE) frontend ARG=$(ARG); then					\
	          (echo -e "$(PASS) $$d \n">>../$(FERESULTS)) && echo "$(PASS) $$d";    \
	    else									\
	          (echo -e "$(FAIL) $$d \n">>../$(FERESULTS)) && echo "$(FAIL) $$d";    \
	    fi;									        \
	    cd ..;	 								\
	done									
#	gedit $(FERESULTS)

# Verify promela Results with spin should be run only after doing make promela
spin:
	echo -e "TESTING WITH Spin ##################\n">$(SPINRESULTS)
	for d in $(SUB_DIRS); 								\
	do										\
	    echo ......................................................; 		\
	    echo Testing $$d with Spin;	 						\
	    echo running with $(ARG) and $(OVERRIDING); 				\
	    cd $$d; 									\
	    ulimit -t $(TIMEOUT);							\
	    $(SPIN) -a final.pr;             				        	\
	    gcc -o pan pan.c;								\
	    if ./pan; then								\
	          (echo -e "$(PASS) $$d \n">>../$(SPINRESULTS)) && echo "$(PASS) $$d";	\
	    else									\
	          (echo -e "$(FAIL) $$d \n">>../$(SPINRESULTS)) && echo "$(FAIL) $$d";	\
	    fi;										\
	    cd ..;	 								\
	done			



# Do make promela (optional)TIMEOUT=.. (optional)ARG=.. and see the generated result file Promela_Results
promela:
	echo -e "TESTING WITH Promela ##################\n">$(PRRESULTS)
	for d in $(SUB_DIRS); 								\
	do 										\
	    echo ......................................................; 		\
	    echo Testing $$d with PROMELA; 						\
	    echo running with $(ARG) and $(OVERRIDING); 				\
	    cd $$d; 									\
	    ulimit -t $(TIMEOUT);							\
	    if $(MAKE) promela ARG=$(ARG); then						\
	          (echo -e "$(PASS) $$d \n">>../$(PRRESULTS)) && echo "$(PASS) $$d";	\
	    else									\
	          (echo -e "$(FAIL) $$d \n">>../$(PRRESULTS)) && echo "$(FAIL) $$d";	\
	    fi;										\
	    cd ..;	 								\
	done									
#	gedit $(PRRESULTS)

# Do make simple (optional)TIMEOUT=.. (optional)ARG=.. and see the generated result file Simple_Results
simple:
	echo -e "TESTING WITH Simple ##################\n">$(SMRESULTS)
	for d in $(SUB_DIRS); 								\
	do										\
	    echo ......................................................; 		\
	    echo Testing $$d with SIMPLE; 						\
	    echo running with $(ARG) and $(OVERRIDING); 				\
	    cd $$d; 									\
	    ulimit -t $(TIMEOUT);							\
	    if $(MAKE) simple ARG=$(ARG); then						\
	        (echo -e "$(PASS) $$d \n">>../$(SMRESULTS)) && echo "$(PASS) $$d";	\
	    else									\
	        (echo -e "$(FAIL) $$d \n">>../$(SMRESULTS)) && echo "$(FAIL) $$d";	\
	    fi;										\
	    cd ..;	 								\
	done
#	gedit $(SMRESULTS)

# Do make test to see if all test examples are getting compiled into bitcode...
test : 
	for d in $(SUB_DIRS); \
	do echo ......................................................; \
	echo Doing make in $$d; \
	$(MAKE) -C $$d; \
	done

# Do make clean to clean all directories
clean : 
	for d in $(SUB_DIRS); \
	do echo Cleaning $$d; \
	$(MAKE) -C $$d $@; \
	done


