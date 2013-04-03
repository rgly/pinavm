/********************************************************************
 * Copyright (C) 2011 by Verimag                                    *
 * Initial author: Matthieu Moy                                     *
 * This file must not be distributed outside Verimag                *
 ********************************************************************/

/*!
  \file BasicHandler.h
  \brief Handler for the BASIC protocol

  
*/
#ifndef BASICHANDLER_H
#define BASICHANDLER_H

#include "SCConstructHandler.hpp"

class BasicHandler : public SCConstructHandler {
public:
	BasicHandler(SCJit * jit):SCConstructHandler(jit) {}
	SCConstruct *handle(Function * fct, BasicBlock * bb, Instruction * callInst, Function* calledFunction);
	void insertInMap(std::map < Function *, SCConstructHandler * >*scchandlers);
};

#endif // BASICHANDLER_H
