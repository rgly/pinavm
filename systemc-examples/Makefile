

OVERRIDING = default

#Default Timeout Value 2 seconds of CPU TIME (can be overriden by command line arguments)
TIMEOUT = 30

#Default Argument Value 3 (can be overriden by command line arguments)
ARG=3

SUB_DIRS = arth_ctrl \
events  \
elab_easy  \
elab_easy_arrays \
elab_easy_int \
elab_easy_uint \
elab_multiple_instances \
elab_ports \
elab_ports_array \
elab_ports_bool \
elab_ports_pointer \
empty-sc \
fifo \
pkt_switch \
RAM \
sc_clock signal \
jerome-chain \
jerome-chain-deterministic \


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
	printf "%s\n\n" "TESTING WITH Frontend ##################">$(FERESULTS)
	for d in $(SUB_DIRS); 								\
	do                                                                      	\
	    echo ......................................................; 		\
	    echo Testing $$d with Frontend; 						\
	    echo running with $(ARG) and $(OVERRIDING); 				\
	    (										\
		cd $$d; 								\
		ulimit -t $(TIMEOUT);							\
		if $(MAKE) frontend ARG=$(ARG); then					\
		      printf "%s\n\n" "$(PASS) $$d" | tee -a ../$(FERESULTS);		\
		else									\
		      printf "%s\n\n" "$(FAIL) $$d" | tee -a ../$(FERESULTS);		\
		fi;									\
	    )										\
	done
#	gedit $(FERESULTS)

# Verify promela Results with spin should be run only after doing make promela
spin:
	printf "%s\n\n" "TESTING WITH Spin ##################">$(SPINRESULTS)
	for d in $(SUB_DIRS); 								\
	do										\
	    echo ......................................................; 		\
	    echo Testing $$d with Spin;	 						\
	    echo running with $(ARG) and $(OVERRIDING); 				\
	    (										\
		cd $$d; 								\
		ulimit -t $(TIMEOUT);							\
		if $(MAKE) spin-run; then						\
		      printf "%s\n\n" "$(PASS) $$d" | tee -a ../$(SPINRESULTS);		\
		else									\
		      printf "%s\n\n" "$(FAIL) $$d" | tee -a ../$(SPINRESULTS);		\
		fi;									\
	    )										\
	done



# Do make promela (optional)TIMEOUT=.. (optional)ARG=.. and see the generated result file Promela_Results
promela:
	printf "%s\n\n" "TESTING WITH Promela ##################">$(PRRESULTS)
	for d in $(SUB_DIRS); 								\
	do 										\
	    echo ......................................................; 		\
	    echo Testing $$d with PROMELA; 						\
	    echo running with $(ARG) and $(OVERRIDING); 				\
	    (										\
		cd $$d; 								\
		ulimit -t $(TIMEOUT);							\
		if $(MAKE) promela ARG=$(ARG); then					\
		      printf "%s\n\n" "$(PASS) $$d" | tee -a ../$(PRRESULTS);		\
		else									\
		      printf "%s\n\n" "$(FAIL) $$d" | tee -a ../$(PRRESULTS);		\
		fi;									\
	    )										\
	done									
#	gedit $(PRRESULTS)

# Do make simple (optional)TIMEOUT=.. (optional)ARG=.. and see the generated result file Simple_Results
simple:
	printf "%s\n\n" "TESTING WITH Simple ##################">$(SMRESULTS)
	for d in $(SUB_DIRS); 								\
	do										\
	    echo ......................................................; 		\
	    echo Testing $$d with SIMPLE; 						\
	    echo running with $(ARG) and $(OVERRIDING); 				\
	    (										\
		cd $$d; 								\
		ulimit -t $(TIMEOUT);							\
		if $(MAKE) simple ARG=$(ARG); then					\
		    printf "%s\n\n" "$(PASS) $$d" | tee -a ../$(SMRESULTS);		\
		else									\
		    printf "%s\n\n" "$(FAIL) $$d" | tee -a ../$(SMRESULTS);		\
		fi;									\
	    )										\
	done
#	gedit $(SMRESULTS)

# Run "make" in all subdirectories.
test :
	for d in $(SUB_DIRS); do \
		echo ......................................................; \
		echo Doing make in $$d; \
		$(MAKE) -C $$d; \
	done

# Do make clean to clean all directories
clean : 
	for d in $(SUB_DIRS); do \
		echo Cleaning $$d; \
		$(MAKE) -C $$d $@; \
	done


