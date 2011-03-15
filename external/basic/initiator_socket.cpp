/********************************************************************
 * Copyright (C) 2011 by Verimag                                    *
 * Initial author: Matthieu Moy                                     *
 * This file must not be distributed outside Verimag                *
 ********************************************************************/

/*!
  \file initiator_socket.cpp
  \brief body for the file initiator_socket.h

  
*/

#include "basic.h"
#include "initiator_socket.h"

namespace basic {
	template<>
	void initiator_socket_base<true>::dummy() {};
	
	template<>
	void initiator_socket_base<false>::dummy() {};

#define CONSTRUCTORS(MULTIPORT)                 \
	template<>                              \
	initiator_socket_base<MULTIPORT>::initiator_socket_base() :	\
		base_type(sc_core::sc_gen_unique_name(kind())),	\
		time(sc_core::SC_ZERO_TIME)     \
	{                                       \
		init();                         \
	}                                       \
	                                        \
	template<>                              \
	initiator_socket_base<MULTIPORT>::initiator_socket_base(const char* name) : \
		base_type(name),                \
		time(sc_core::SC_ZERO_TIME)     \
	{                                       \
		init();                         \
	}

	CONSTRUCTORS(true);
	CONSTRUCTORS(false);

}
