To create your own Handler:
    Name your Handler in [NAME]Handler.[h,hpp] format.
    All Handlers must public inherit SCConstructHandler.
    you should implement "insertInMap()" & "handle()" methods.


To create your own Construct:
    Name your Handler in [NAME]Construct.[h,hpp] format.
    All Constructs must public inherit SCConstruct.


To include :
    use #include "ALLConstruct.h" , which includes all the constructs.



The handlers are defined by this inheritance tree :

- SCConstructHandler
  |- EventWaitHandler
  |- TimeWaitHandler
  |- DeltaWaitHandler
  |- ReadHandler
  |- WriteHandler
  |- NotifyHandler 
  |- RandHandler 
  '- AssertHandler 

A SystemC construct is the result of the "handle()" method of a
handler. The representation of SystemC constructs is given by the
very-similar-to-handlers tree:

- SCConstruct
  |- WaitConstruct
  |  |- EventWaitConstruct
  |  |- TimeWaitConstruct
  |  '- DeltaWaitConstruct
  |- NotifyConstruct
  |- WriteConstruct
  |- ReadConstruct
  |- RandConstruct
  '- AssertConstruct

These constructs use the classes:
- SCPort
- SCEvent
- SCModule

which should be redefined with respect to ELAB.  
