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

struct transition{
  int    numStateSink;
  string transitionLab;
};

struct state{
  int  InitialFinal;
  int  numState;
  list<transition> *pointerListTransition; 
};

class _42AutomatonContract {
private:
  list<state> listState;
  bool existNotify;
  bool existWait;
  vector<string> eventsWaited;
  vector<string> eventsNotified;
  int lastBuildState;
public:
  _42AutomatonContract();
  int  addState(int isInitial);
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


