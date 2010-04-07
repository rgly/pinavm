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
public:
  _42AutomatonContract();

  void set_stateStatus(int _numState,StateStatus _Status);

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
};

#endif


