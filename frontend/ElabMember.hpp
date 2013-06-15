#ifndef _ELABMEMBER_HPP
#define _ELABMEMBER_HPP

#include <string>
struct SCElab;

class ElabMember {
protected:
    ElabMember() : elab(NULL){};
    ElabMember(const SCElab* el) : elab(el){};
    // virtual destructor here in order to keep compiler not warning.
    virtual ~ElabMember() {};
    void printPrefix(int sep, std::string prefix);
    virtual void printElab(int sep, std::string prefix) = 0;
    const SCElab* getElab() const {return this->elab;};
private:
    const SCElab* elab;
};

#endif
