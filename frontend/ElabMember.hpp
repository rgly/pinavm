#ifndef _ELABMEMBER_HPP
#define _ELABMEMBER_HPP

#include <string>

class ElabMember {
protected:
    // virtual destructor here in order to keep compiler not warning.
    virtual ~ElabMember() {};
    void printPrefix(int sep, std::string prefix);
    virtual void printElab(int sep, std::string prefix) = 0;
};

#endif
