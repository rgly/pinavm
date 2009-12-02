#ifndef _ELABMEMBER_HPP
#define _ELABMEMBER_HPP

#include <string>

class ElabMember
{
protected:
  void printPrefix(int sep, std::string prefix);
  virtual void printElab(int sep, std::string prefix) = 0;
};

#endif
