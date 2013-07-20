########################################
####### BUILD and USE information ######
########################################

See the doc/INSTALL file

####################################
############# FILES ################
####################################

*** 
A class is defined in a <classname>.hpp file and and its methods
implemented in the corresponding <classname>.cpp file
*** 

- FunctionNames is the LLVM pass (understand: is the entry point of the
tool for the moment). It inherits from the LLVM class FunctionPassand
therefore define the "runOnFunction" method which will be executed on
each function of the given bitcode file.

- SCJit is the class constructing functions and jitting them.

- SCCFactory mainly associates one Handler to a Function*


##############################################
################ NOTES #######################
##############################################

- The name "FunctionNames" is really not accurate (I started from a
small pass of my own). I propose to change it once a name has been
found. If we keep "pinapa", renaming the file "PinapaPass" would be
clearer.
