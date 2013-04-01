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

The handlers are defined by this inheritance tree :

- SCConstructHandler
  |- EventHandler
  |- ReadHandler
  |- WriteHandler
  |- NotifyHandler 
  |- TimeHandler
     |- ZeroTimeHandler
     |- DefaultTimeHandler

A SystemC construct is the result of the "handle()" method of a
handler. The representation of SystemC constructs is given by the
very-similar-to-handlers tree:

- SCConstruct
  |- NotifyConstruct
  |- WriteConstruct
  |- ReadConstruct
  |- TimeConstruct
     |- ZeroTimeConstruct
     |- DefaultTimeConstruct

These constructs use the classes:
- SCPort
- SCEvent
- SCModule

which should be redefined with respect to ELAB.  

##############################################
################ NOTES #######################
##############################################

- The name "FunctionNames" is really not accurate (I started from a
small pass of my own). I propose to change it once a name has been
found. If we keep "pinapa", renaming the file "PinapaPass" would be
clearer.
