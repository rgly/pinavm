#ifndef _42AUTOMATONCONTRACT_H
#define _42AUTOMATONCONTRACT_H

#include <list>
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
  StateStatus  Status;   /* type enumere */
  int  numState;
  vector<transition> *pointerListTransition; /* mieux vaut mettre un vector */
};

class _42AutomatonContract {
private:
  vector<state> listState;
  bool existNotify;
  bool existWait;
  vector<string> eventsWaited;
  vector<string> eventsNotified;
  int lastBuildState;
public:
  _42AutomatonContract();
  int  addState(StateStatus Status);
  void addTransition(int numeStateSrc, int numStateSink, string transitionLab);
  void printDrawContract(formatted_raw_ostream &o);
  bool get_existNotify();
  void set_existNotify(bool exist);
  bool get_existWait();
  void set_existWait(bool exist);
  int get_lastBuildState();
  void set_lastBuildState(int number);
  string toString_eventsWaited();
  string toString_eventsNotified();
  void clearEventsWaited();
  void clearEventsNotified();
  void pushEventWaited(string event);
  void pushEventNotified(string event);
};

#endif


