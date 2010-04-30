#ifndef _42AUTOMATONCONTRACT_H
#define _42AUTOMATONCONTRACT_H

#include <set>
#include <string>
#include <vector>
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileUtilities.h"
#include "llvm/Support/FormattedStream.h"

using namespace std;
using namespace llvm;


typedef enum{Initial,Intermediate,Final} StateStatus;
//typedef enum{Wait,Notify,Undefined} FirstSCConstruct;

struct transition{
  int    numStateSink;
  string transitionLab;
};

struct state{
  StateStatus  Status;  
  int  numState;
  vector<transition> *pointerListTransition;
};

class _42AutomatonContract {
private:
  vector<state> listState;
  bool existNotify;
  bool existWait;
  vector<string> eventsWaited;
  vector<string> eventsNotified;
  int lastBuildState;
  bool structIfElseDetectedJustBefore;
  set<string> BBVisited;
  string nameBasicBlockAfterIf;
  //string nameFirstBasicBlockIf;
  bool visitingIfBranch;
  bool visitingElseBranch;
  bool visitingWhileBranch;
  string lastBasicBlock;
  bool existReturnInstInIfBranch;
  bool existReturnInstInElseBranch;
  bool whileDetected;
  bool endWhileDetected;
  //  FirstSCConstruct SCConstruct;
  /*  bool isEmptyEventsWaited;
      bool isEmptyEventsNotified;*/
public:
  _42AutomatonContract();

  void set_stateStatus(int _numState,StateStatus _Status);

  //  FirstSCConstruct get_firstSCConstruct();
  //void set_firstSCConstruct(FirstSCConstruct _SCConstruct);

  bool get_visitingIfBranch();
  void set_visitingIfBranch(bool visiting);

  bool get_visitingElseBranch();
  void set_visitingElseBranch(bool visiting);

  bool get_visitingWhileBranch();
  void set_visitingWhileBranch(bool visiting);

  bool get_existReturnInstInIfBranch();
  void set_existReturnInstInIfBranch(bool exist);

  bool get_existReturnInstInElseBranch();
  void set_existReturnInstInElseBranch(bool exist);

  bool get_whileDetected();
  void set_whileDetected(bool detected);

  bool get_endWhileDetected();
  void set_endWhileDetected(bool detected);

  bool get_existNotify();
  void set_existNotify(bool exist);

  bool get_existWait();
  void set_existWait(bool exist);

  int  get_lastBuildState();
  void set_lastBuildState(int number);

  bool get_structIfElseDetectedJustBefore();
  void set_structIfElseDetectedJustBefore(bool detected);

  vector<string> get_eventsWaited();
  void           set_eventsWaited(vector<string> eventsWaited);

  vector<string> get_eventsNotified();
  void           set_eventsNotified(vector<string> eventsNotified);

  string get_nameBasicBlockAfterIf();
  void   set_nameBasicBlockAfterIf(string name);

  //string get_nameFirstBasicBlockIf();
  //void   set_nameFirstBasicBlockIf(string name);

  string toString_eventsWaited();
  string toString_eventsNotified();

  void clearEventsWaited();
  void clearEventsNotified();

  void pushEventWaited  (string event);
  void pushEventNotified(string event);

  int  addState(StateStatus Status);
  void addTransition(int numStateSrc, int numStateSink, string transitionLab);
  
  void printDrawContract(formatted_raw_ostream &o);

  void addBasicBlockVisited(string name);
  bool isBasicBlockAlreadyVisited(string name);

  string get_lastBasicBlock();
  void   set_lastBasicBlock(string name);

  /*  bool isEmptyEventsWaited();
      bool isEmptyEventsNotified();*/
};

#endif


