/********************************************************************
 * Copyright (C) 2011 by Verimag                                    *
 * Initial author: Matthieu Moy                                     *
 * This file must not be distributed outside Verimag                *
 ********************************************************************/

/*!
  \file target_socket.cpp
  \brief body for the file target_socket.h

  
*/

#include "basic.h"
#include "target_socket.h"

namespace basic {
	target_module_base::target_module_base() {};
	void target_module_base::dummy(){};
	
#define CONSTRUCTORS(MULTIPORT)			\
	template<>				\
	target_socket_base<MULTIPORT>::target_socket_base() :	\
		base_type(sc_core::sc_gen_unique_name(kind()))	\
	{					\
		init();				\
	};					\
						\
	template<>				\
	target_socket_base<MULTIPORT>::target_socket_base(const char* name) :	\
		base_type(name)			\
	{					\
		init();				\
	}					\

	CONSTRUCTORS(true);
	CONSTRUCTORS(false);
	
};
