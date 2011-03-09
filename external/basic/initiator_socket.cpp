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

	void initiator_socket_base_true::dummy() {};
}
