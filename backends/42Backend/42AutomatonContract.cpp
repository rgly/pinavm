#include <set>
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
  structIfElseDetectedJustBefore=false;
  visitingIfBranch=false;
  visitingElseBranch=false;
  visitingWhileBranch=false;
  existReturnInstInIfBranch=false;
  existReturnInstInElseBranch=false;
  whileDetected=false;
  endWhileDetected=false;
  nameBasicBlockAfterIf="";
  // nameFirstBasicBlockIf="";
  // SCConstruct=Undefined;
}

void _42AutomatonContract::set_stateStatus(int _numState,StateStatus _Status){
  vector<state>::iterator it;

  it=(this->listState).begin();

  while(it->numState!=_numState){
    it++;
  }

  it->Status=_Status;
}

/*FirstSCConstruct _42AutomatonContract::get_firstSCConstruct(){
  return this-> SCConstruct;
}

void set_firstSCConstruct(FirstSCConstruct _SCConstruct){
  this->SCConstruct=_SCConstruct;
  }*/

bool _42AutomatonContract::get_visitingIfBranch(){
  return this->visitingIfBranch;
}

void _42AutomatonContract::set_visitingIfBranch(bool visiting){
  this->visitingIfBranch=visiting;
}

bool _42AutomatonContract::get_visitingElseBranch(){
  return this->visitingElseBranch;
}

void _42AutomatonContract::set_visitingElseBranch(bool visiting){
  this->visitingElseBranch=visiting;
}

bool _42AutomatonContract::get_visitingWhileBranch(){
  return this->visitingWhileBranch;
}

void _42AutomatonContract::set_visitingWhileBranch(bool visiting){
  this->visitingWhileBranch=visiting;
}

bool _42AutomatonContract::get_existReturnInstInIfBranch(){
  return this->existReturnInstInIfBranch;
}

void _42AutomatonContract::set_existReturnInstInIfBranch(bool exist){
  this->existReturnInstInIfBranch=exist;
}

bool _42AutomatonContract::get_existReturnInstInElseBranch(){
  return this->existReturnInstInElseBranch;
}

void _42AutomatonContract::set_existReturnInstInElseBranch(bool exist){
  this->existReturnInstInElseBranch=exist;
}

bool _42AutomatonContract::get_whileDetected(){
  return this->whileDetected;
}

void _42AutomatonContract::set_whileDetected(bool detected){
  this->whileDetected=detected;
}

bool _42AutomatonContract::get_endWhileDetected(){
  return this->endWhileDetected;
}

void _42AutomatonContract::set_endWhileDetected(bool detected){
  this->endWhileDetected=detected;
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

bool _42AutomatonContract::get_structIfElseDetectedJustBefore(){
  return this->structIfElseDetectedJustBefore;
}

void _42AutomatonContract::set_structIfElseDetectedJustBefore(bool detected){
  this->structIfElseDetectedJustBefore=detected;
}

vector<string> _42AutomatonContract::get_eventsWaited(){
  return this->eventsWaited;
}

void _42AutomatonContract::set_eventsWaited(vector<string> eventsWaited){
  for(unsigned int i(0);i<(eventsWaited).size();++i){
    pushEventWaited(eventsWaited[i]);
  }
}

vector<string> _42AutomatonContract::get_eventsNotified(){
  return this->eventsNotified;
}

void _42AutomatonContract::set_eventsNotified(vector<string> eventsNotified){
  for(unsigned int i(0);i<(eventsNotified).size();++i){
    pushEventNotified(eventsNotified[i]);
  }
}

string _42AutomatonContract::get_nameBasicBlockAfterIf(){
  string s;
  s=this->nameBasicBlockAfterIf;
  return s;
}

void _42AutomatonContract::set_nameBasicBlockAfterIf(string name){
  this->nameBasicBlockAfterIf=name;
}
  
/*string _42AutomatonContract::get_nameFirstBasicBlockIf(){
  string s;
  s=this->nameFirstBasicBlockIf;
  return s;
}

void _42AutomatonContract::set_nameFirstBasicBlockIf(string name){
  this->nameFirstBasicBlockIf=name;
  }*/

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

void _42AutomatonContract::addBasicBlockVisited(string name){
  (this->BBVisited).insert(name);
}

bool _42AutomatonContract::isBasicBlockAlreadyVisited(string name){
  if((this->BBVisited).find(name)==(this->BBVisited).end())
    return false;
  else
    return true;
}

/*bool _42AutomatonContract::isEmptyEventsWaited(){
  return this->eventsWaited.empty();
}

bool _42AutomatonContract::isEmptyEventsNotified(){
  return this->eventsNotified.empty();
  }*/


string _42AutomatonContract::get_lastBasicBlock(){
  string s;
  s=this->lastBasicBlock;
  return s;
}

void _42AutomatonContract::set_lastBasicBlock(string name){
  this->lastBasicBlock=name;
}






