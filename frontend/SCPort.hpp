#ifndef _SCPORT_HPP
#define _SCPORT_HPP

#include <string>

using namespace std;

struct SCPort {
private:
  SCModule SCModule;
  SCModule bindedSCModule;

  string name;

public:
  SCPort(SCModule* module, string portName);
  SCModule* getSCModule();

  SCModule* getBindedModule();
  void setBindedModule(SCModule*);

  string getPortName();
}
;
#endif
