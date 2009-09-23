#ifndef PORT_HPP
#define PORT_HPP

#include <string>

using namespace std;

struct Port {
private:
  string moduleName;
  string portName;
public:
  Port(string module, string port);
}
;
#endif
