#ifndef _WAITCONSTRUCT_HPP
#define _WAITCONSTRUCT_HPP

#include "SCConstruct.hpp"

enum WaitIDTy {
    TIMED,
    EVENT,
    DELTA
};

class WaitConstruct : public SCConstruct
{
public:
    virtual ~WaitConstruct(){};
    WaitIDTy getWaitID() const;
    static inline bool classof(const SCConstruct *scc) {
        return (scc->getID() == WAITCONSTRUCT);
    }

protected:
    WaitConstruct(WaitIDTy, bool);
    WaitConstruct(WaitIDTy);

private:
    WaitIDTy WaitID;
};
#endif
