/********************************************************************
 * Copyright (C) 2011 by Verimag                                    *
 * Initial author: Matthieu Moy                                     *
 * This file must not be distributed outside Verimag                *
 ********************************************************************/

/*!
  \file BasicHandler.cpp
  \brief body for the file BasicHandler.h

  
*/

#include "BasicHandler.h"

#include <llvm/Support/CallSite.h>

#include "SCJit.hpp"
#include "Port.hpp"
#include "Channel.hpp"
#include "WriteConstruct.hpp"
#include "basic.h"

SCConstruct *BasicHandler::handle(Function * fct, BasicBlock * bb, Instruction* callInst, Function* calledFunction)
{
	TRACE_3("Handling call to basic write()\n");
	WriteConstruct* ret;
	// Analyzing "arg.write(value)"
	Value *arg = CallSite(callInst).getArgument(0);
	Value *value = CallSite(callInst).getArgument(1);
	Port *po = NULL;

	void *portAddr = this->scjit->jitAddr(fct, callInst, arg);

	TRACE_4("Address jitted : " << portAddr << "\n");
	if (portAddr == NULL)
		ret = new WriteConstruct(arg);
	else {
		// We've got a void *, we can't do better than reinterpret_cast
		basic::initiator_socket_base<false> *basicPortAddr = 
			reinterpret_cast<basic::initiator_socket_base<false> *>(portAddr);
		// multiple inheritence => this dymanic_cast may
		// change the address
		sc_core::sc_port_base *portBaseAddr =
			dynamic_cast<sc_core::sc_port_base *>(basicPortAddr);
		po = this->scjit->getElab()->getPort(portBaseAddr);
		ret = new WriteConstruct(po);
		TRACE_3("Port jit-ed : " << basicPortAddr->name() << "\n");
		TRACE_3("Port written : " << po->getName() << "\n");
	}

	return ret;
	// TODO: handle basic read()
}

void BasicHandler::insertInMap(std::map < Function *, SCConstructHandler * >*scchandlers)
{
	// basic::initiator_socket_base<false>::write(unsigned int const&, unsigned int, int)
	SCConstructHandler::insertInMap(scchandlers, "_ZN5basic21initiator_socket_baseILb0EE5writeERKjji");
	// basic::initiator_socket_base<true>::write(unsigned int const&, unsigned int, int)
	SCConstructHandler::insertInMap(scchandlers, "_ZN5basic21initiator_socket_baseILb1EE5writeERKjji");
}
