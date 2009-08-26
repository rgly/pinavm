#ifndef PORT_HPP
#define PORT_HPP

struct Port {
private:
  char* moduleName;
  char* portName;
public:
  Port(char* module, char* port);
}
;
#endif
