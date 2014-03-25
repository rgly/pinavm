/*****************************************************************************

  The following code is derived, directly or indirectly, from the SystemC
  source code Copyright (c) 1996-2006 by all Contributors.
  All Rights reserved.

  The contents of this file are subject to the restrictions and limitations
  set forth in the SystemC Open Source License Version 2.4 (the "License");
  You may not use this file except in compliance with such restrictions and
  limitations. You may obtain instructions on how to receive a copy of the
  License at http://www.systemc.org/. Software distributed by Contributors
  under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
  ANY KIND, either express or implied. See the License for the specific
  language governing rights and limitations under the License.

 *****************************************************************************/

/*****************************************************************************

  veri_signal.cpp -- The veri_signal<T> primitive channel class.

 *****************************************************************************/



#include "sysc/communication/sc_communication_ids.h"
#include "sysc/utils/sc_utils_ids.h"
#include "sysc/communication/veri_signal.h"
#include "sysc/datatypes/int/sc_signed.h"
#include "sysc/datatypes/int/sc_unsigned.h"
#include "sysc/datatypes/bit/sc_lv_base.h"
#include "sysc/kernel/sc_reset.h"

using sc_dt::sc_lv_base;
using sc_dt::sc_signed;
using sc_dt::sc_unsigned;
using sc_dt::int64;
using sc_dt::uint64;

namespace sc_core {

void
veri_signal_invalid_writer( 
    sc_object* target, sc_object* first_writer, sc_object* second_writer )
{
    char msg[BUFSIZ];
    const char* target_name = target->name();
    const char* target_kind = target->kind();
    const char* writer1_name = first_writer->name();
    const char* writer1_kind = first_writer->kind();
    const char* writer2_name;
    const char* writer2_kind;
    if ( second_writer )
    {
        writer2_name = second_writer->name();
        writer2_kind = second_writer->kind();

	std::sprintf( msg, "\n signal `%s' (%s)"
	     "\n first driver `%s' (%s)"
	     "\n second driver `%s' (%s)",
	     target_name, target_kind, 
	     writer1_name, writer1_kind, 
	     writer2_name, writer2_kind );
	SC_REPORT_ERROR( SC_ID_MORE_THAN_ONE_SIGNAL_DRIVER_, msg );
    }
}


// ----------------------------------------------------------------------------
//  CLASS : veri_signal<bool>
//
//  Specialization of veri_signal<T> for type bool.
// ----------------------------------------------------------------------------


// reset support:

sc_reset* veri_signal<bool>::is_reset() const
{
    sc_reset* result_p;
    if ( !m_reset_p ) m_reset_p = new sc_reset( this );
    result_p = m_reset_p;
    return result_p;
}

// destructor

veri_signal<bool>::~veri_signal()
{
    if ( !m_change_event_p )  delete m_change_event_p;
    if ( !m_negedge_event_p ) delete m_negedge_event_p;
    if ( !m_posedge_event_p ) delete m_posedge_event_p;
    if ( m_reset_p )          delete m_reset_p;
}

} // namespace sc_core

// Taf!
