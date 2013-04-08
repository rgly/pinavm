// -*- c-basic-offset: 3 -*-
#ifndef BASIC_H
#define BASIC_H

#include <systemc>
#include <tlm.h>
#include <stdint.h>

namespace basic {
   typedef uint32_t addr_t;
   typedef uint32_t data_t;
}

#include "initiator_socket.h"
#include "target_socket.h"

#endif
