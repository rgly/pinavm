#include <list>
#include <vector>
#include <iostream>
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileUtilities.h"
#include "llvm/Support/FormattedStream.h"
#include "42AutomatonContract.h"

using namespace llvm;
  

_42AutomatonContract::_42AutomatonContract(){
  existNotify=false;
  existWait=false;
}

int _42AutomatonContract::addState(StateStatus _Status){
  state newState;

  newState.Status=_Status;
  newState.numState=this->listState.size();
  newState.pointerListTransition=new vector<transition>;

  (this->listState).push_back(newState);

  return newState.numState;
}

void _42AutomatonContract::addTransition(int _numStateSrc, int _numStateSink, string _transitionLab){
  transition newTransition;
  vector<state>::iterator it;

  newTransition.numStateSink=_numStateSink;
  newTransition.transitionLab=_transitionLab;

  it=(this->listState).begin();

  while(it->numState!=_numStateSrc){
    it++;
  }

  (*(it->pointerListTransition)).push_back(newTransition);
}

void _42AutomatonContract::printDrawContract(formatted_raw_ostream &o){
  vector<state>::iterator it1;
  vector<transition>::iterator it2;

  for(it1=(this->listState).begin(); it1!=(this->listState).end(); it1++){
    o << "STATE_" << it1->numState;
    
    if((it1->Status)==Initial)
      o << " (initial) : \n";
    else{
      if((it1->Status)==Final)
	o << " (final) : \n";
      else
	o << " : \n";
    }

    for(it2=(*(it1->pointerListTransition)).begin(); it2!=(*(it1->pointerListTransition)).end(); it2++){
      o << "----> STATE_" << it2->numStateSink << " : " << it2->transitionLab << "\n";
    }
  }
}


bool _42AutomatonContract::get_existNotify(){
  return this->existNotify;
}

void _42AutomatonContract::set_existNotify(bool exist){
  this->existNotify=exist;
}

bool _42AutomatonContract::get_existWait(){
  return this->existWait;
}

void _42AutomatonContract::set_existWait(bool exist){
  this->existWait=exist;
}

int _42AutomatonContract::get_lastBuildState(){
  return this->lastBuildState;
}

void _42AutomatonContract::set_lastBuildState(int number){
  this->lastBuildState=number;
}

string _42AutomatonContract::toString_eventsWaited(){
  string s;

  for(unsigned int i(0);i<(this->eventsWaited).size();++i){
    s=s+(this->eventsWaited)[i]+";";
  }

  return s;
}
  
string _42AutomatonContract::toString_eventsNotified(){
  string s;

  for(unsigned int i(0);i<(this->eventsNotified).size();++i){
    s=s+(this->eventsNotified)[i]+";";
  }

  return s;
}

void _42AutomatonContract::clearEventsWaited(){
  (this->eventsWaited).clear();
}

void _42AutomatonContract::clearEventsNotified(){
  (this->eventsNotified).clear();
}

void _42AutomatonContract::pushEventWaited(string event){
  (this->eventsWaited).push_back(event);
}

void _42AutomatonContract::pushEventNotified(string event){
  (this->eventsNotified).push_back(event);
}




