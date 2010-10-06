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

  veri_signal.h -- The veri_signal<T> primitive channel class.

  Modification log appears at the end of the file
 *****************************************************************************/


#ifndef VERI_SIGNAL_H
#define VERI_SIGNAL_H


#include "sysc/communication/sc_port.h"
#include "sysc/communication/sc_prim_channel.h"
#include "sysc/communication/sc_signal_ifs.h"
#include "sysc/utils/sc_string.h"
#include "sysc/kernel/sc_event.h"
#include "sysc/kernel/sc_process.h"
#include "sysc/kernel/sc_reset.h"
#include "sysc/kernel/sc_simcontext.h"
#include "sysc/datatypes/bit/sc_logic.h"
#include "sysc/tracing/sc_trace.h"
#include <typeinfo>

namespace sc_core {

extern
void
veri_signal_invalid_writer(
    sc_object* target, sc_object* first_writer, sc_object* second_writer );


// ----------------------------------------------------------------------------
//  CLASS : veri_signal<T>
//
//  The veri_signal<T> primitive channel class.
// ----------------------------------------------------------------------------

template <class T>
class veri_signal
: public sc_signal_inout_if<T>,
  public sc_prim_channel
{
  public: // constructors and destructor:

    veri_signal()
	: sc_prim_channel( sc_gen_unique_name( "veri_signal" ) ),
	  m_change_event_p( 0 ), m_cur_val( T() ), m_output( 0 ), 
	  m_writer( 0 )
	{}

    explicit veri_signal( const char* name_ )
	: sc_prim_channel( name_ ),
	  m_change_event_p( 0 ), m_cur_val( T() ), 
	  m_output( 0 ), 
	  m_writer( 0 ) 
    {}


    virtual ~veri_signal()
	{
	    if ( !m_change_event_p ) delete m_change_event_p;
	}


    // interface methods

    virtual void register_port( sc_port_base&, const char* );


    // get the default event
    virtual const sc_event& default_event() const
	{ 
	    if ( !m_change_event_p ) m_change_event_p = new sc_event; 
	    return *m_change_event_p; 
	}


    // get the value changed event
    virtual const sc_event& value_changed_event() const
	{ 
	    if ( !m_change_event_p ) m_change_event_p = new sc_event; 
	    return *m_change_event_p; 
	}


    // read the current value
    virtual const T& read() const
	{ return m_cur_val; }

    // get a reference to the current value (for tracing)
    virtual const T& get_data_ref() const
        { return m_cur_val; }


    // was there an event?
    virtual bool event() const
        { return simcontext()->event_occurred(m_delta); }

    // write the new value
    virtual void write( const T& );


    // other methods

    operator const T& () const
	{ return read(); }


    veri_signal<T>& operator = ( const T& a )
	{ write( a ); return *this; }

    veri_signal<T>& operator = ( const veri_signal<T>& a )
	{ write( a.read() ); return *this; }


    virtual void print( ::std::ostream& = ::std::cout ) const;
    virtual void dump( ::std::ostream& = ::std::cout ) const;

    virtual const char* kind() const
        { return "veri_signal"; }


protected:

    mutable sc_event*  m_change_event_p;
    T                  m_cur_val;
    sc_dt::uint64      m_delta;   // delta of last event
    sc_port_base*      m_output; // used for static design rule checking
    sc_object*         m_writer; // used for dynamic design rule checking



private:

    // disabled
    veri_signal( const veri_signal<T>& );
};


// IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII


template <class T>
inline
void
veri_signal<T>::register_port( sc_port_base& port_, const char* if_typename_ )
{
    if ( sc_get_curr_simcontext()->write_check() )
    {
	std::string nm( if_typename_ );
	if( nm == typeid( sc_signal_inout_if<T> ).name() ) {
	    // an out or inout port; only one can be connected
	    if( m_output != 0) {
		veri_signal_invalid_writer( this, m_output, &port_ );
	    }
	    m_output = &port_;
	}
    }
}


// write the new value

template <class T>
inline
void
veri_signal<T>::write( const T& value_ )
{
    sc_object* writer = sc_get_curr_simcontext()->get_current_writer();
    if( m_writer == 0 ) {
	m_writer = writer;
    } else if( m_writer != writer ) {
	veri_signal_invalid_writer( this, m_writer, writer );
    }

    m_cur_val = value_;
}


template <class T>
inline
void
veri_signal<T>::print( ::std::ostream& os ) const
{
    os << m_cur_val;
}

template <class T>
inline
void
veri_signal<T>::dump( ::std::ostream& os ) const
{
    os << "     name = " << name() << ::std::endl;
    os << "    value = " << m_cur_val << ::std::endl;
}


// ----------------------------------------------------------------------------
//  CLASS : veri_signal<bool>
//
//  Specialization of veri_signal<T> for type bool.
// ----------------------------------------------------------------------------

class sc_reset;

template <>
class veri_signal<bool>
: public sc_signal_inout_if<bool>,
  public sc_prim_channel
{
public: // constructors and destructor:

    veri_signal()
	: sc_prim_channel( sc_gen_unique_name( "signal" ) ),
	  m_change_event_p( 0 ),
          m_cur_val( false ),
          m_delta( ~sc_dt::UINT64_ONE ),
	  m_negedge_event_p( 0 ),
          m_output( 0 ),
	  m_posedge_event_p( 0 ),
          m_reset_p( 0 ),
          m_writer( 0 )
	{}

    explicit veri_signal( const char* name_ )
	: sc_prim_channel( name_ ),
	  m_change_event_p( 0 ),
          m_cur_val( false ),
          m_delta( ~sc_dt::UINT64_ONE ),
	  m_negedge_event_p( 0 ),
          m_output( 0 ),
	  m_posedge_event_p( 0 ),
          m_reset_p( 0 ),
          m_writer( 0 )
	{}

    virtual ~veri_signal();


    // interface methods

    virtual void register_port( sc_port_base&, const char* );


    // get the default event
    virtual const sc_event& default_event() const
	{ 
	    if ( !m_change_event_p ) m_change_event_p = new sc_event; 
	    return *m_change_event_p; 
	}


    // get the value changed event
    virtual const sc_event& value_changed_event() const
	{ 
	    if ( !m_change_event_p ) m_change_event_p = new sc_event; 
	    return *m_change_event_p; 
	}

    // get the positive edge event
    virtual const sc_event& posedge_event() const
	{ 
	    if ( !m_posedge_event_p )
	        m_posedge_event_p = new sc_event; 
	    return *m_posedge_event_p; 
	}

    // get the negative edge event
    virtual const sc_event& negedge_event() const
	{ 
	    if ( !m_negedge_event_p )
	        m_negedge_event_p = new sc_event; 
	    return *m_negedge_event_p; 
	}


    // read the current value
    virtual const bool& read() const
	{ return m_cur_val; }

    // get a reference to the current value (for tracing)
    virtual const bool& get_data_ref() const
        { return m_cur_val; }


    // was there a value changed event?
    virtual bool event() const
        { return simcontext()->event_occurred(m_delta); }

    // was there a positive edge event?
    virtual bool posedge() const
	{ return ( event() && m_cur_val ); }

    // was there a negative edge event?
    virtual bool negedge() const
	{ return ( event() && ! m_cur_val ); }

    // reset creation

    virtual sc_reset* is_reset() const;

    // write the new value
    virtual void write( const bool& );

    // other methods

    operator const bool& () const
	{ return read(); }


    veri_signal<bool>& operator = ( const bool& a )
	{ write( a ); return *this; }

    veri_signal<bool>& operator = ( const veri_signal<bool>& a )
	{ write( a.read() ); return *this; }


    virtual void print( ::std::ostream& = ::std::cout ) const;
    virtual void dump( ::std::ostream& = ::std::cout ) const;

    virtual const char* kind() const
        { return "veri_signal"; }

protected:

    virtual bool is_clock() const { return false; }

protected:
    mutable sc_event* m_change_event_p;  // value change event if present.
    bool              m_cur_val;         // current value of object.
    sc_dt::uint64     m_delta;           // delta of last event
    mutable sc_event* m_negedge_event_p; // negative edge event if present.
    sc_port_base*     m_output;          // used for static design rule checking
    mutable sc_event* m_posedge_event_p; // positive edge event if present.
    mutable sc_reset* m_reset_p;         // reset mechanism if present.
    sc_object*        m_writer;          // process writing this object's value.

private:

    // disabled
    veri_signal( const veri_signal<bool>& );
};


// IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII

inline
void
veri_signal<bool>::register_port( sc_port_base& port_, const char* if_typename_ )
{
    if ( sc_get_curr_simcontext()->write_check() )
    {
	std::string nm( if_typename_ );
	if( nm == typeid( sc_signal_inout_if<bool> ).name() ) {
	    // an out or inout port; only one can be connected
	    if( m_output != 0 ) {
		veri_signal_invalid_writer( this, m_output, &port_ );
	    }
	    m_output = &port_;
	}
    }
}


// write the new value

inline
void
veri_signal<bool>::write( const bool& value_ )
{
    sc_object* writer = sc_get_curr_simcontext()->get_current_writer();
    if( m_writer == 0 ) {
	m_writer = writer;
    } else if( m_writer != writer ) {
	veri_signal_invalid_writer( this, m_writer, writer );
    }

    m_cur_val = value_;
}


inline
void
veri_signal<bool>::print( ::std::ostream& os ) const
{
    os << m_cur_val;
}

inline
void
veri_signal<bool>::dump( ::std::ostream& os ) const
{
    os << "     name = " << name() << ::std::endl;
    os << "    value = " << m_cur_val << ::std::endl;
}


// ----------------------------------------------------------------------------
//  CLASS : veri_signal<sc_dt::sc_logic>
//
//  Specialization of veri_signal<T> for type sc_dt::sc_logic.
// ----------------------------------------------------------------------------

template <>
class veri_signal<sc_dt::sc_logic>
: public sc_signal_inout_if<sc_dt::sc_logic>,
  public sc_prim_channel
{
  public: // constructors and destructor:

    veri_signal()
	: sc_prim_channel( sc_gen_unique_name( "signal" ) ),
	  m_change_event_p( 0 ),
	  m_cur_val(),
          m_delta( ~sc_dt::UINT64_ONE ),
	  m_negedge_event_p( 0 ),
          m_output( 0 ),
	  m_posedge_event_p( 0 ),
	  m_writer( 0 )
	{}

    explicit veri_signal( const char* name_ )
	: sc_prim_channel( name_ ),
	  m_change_event_p( 0 ),
	  m_cur_val(),
          m_delta( ~sc_dt::UINT64_ONE ),
	  m_negedge_event_p( 0 ),
          m_output( 0 ),
	  m_posedge_event_p( 0 ),
	  m_writer( 0 )
	{}

    virtual ~veri_signal()
	{
	    if ( !m_change_event_p ) delete m_change_event_p;
	    if ( !m_negedge_event_p ) delete m_negedge_event_p;
	    if ( !m_posedge_event_p ) delete m_posedge_event_p;
	}


    // interface methods

    virtual void register_port( sc_port_base&, const char* );


    // get the default event
    virtual const sc_event& default_event() const
	{ 
	    if ( !m_change_event_p ) m_change_event_p = new sc_event; 
	    return *m_change_event_p; 
	}


    // get the value changed event
    virtual const sc_event& value_changed_event() const
	{ 
	    if ( !m_change_event_p ) m_change_event_p = new sc_event; 
	    return *m_change_event_p; 
	}

    // get the positive edge event
    virtual const sc_event& posedge_event() const
	{ 
	    if ( !m_posedge_event_p )
	        m_posedge_event_p = new sc_event; 
	    return *m_posedge_event_p; 
	}

    // get the negative edge event
    virtual const sc_event& negedge_event() const
	{ 
	    if ( !m_negedge_event_p )
	        m_negedge_event_p = new sc_event; 
	    return *m_negedge_event_p; 
	}


    // read the current value
    virtual const sc_dt::sc_logic& read() const
	{ return m_cur_val; }

    // get a reference to the current value (for tracing)
    virtual const sc_dt::sc_logic& get_data_ref() const
        { return m_cur_val; }


    // was there an event?
    virtual bool event() const
        { return simcontext()->event_occurred(m_delta); }

    // was there a positive edge event?
    virtual bool posedge() const
	{ return ( event() && m_cur_val == sc_dt::SC_LOGIC_1 ); }

    // was there a negative edge event?
    virtual bool negedge() const
	{ return ( event() && m_cur_val == sc_dt::SC_LOGIC_0 ); }


    // write the new value
    virtual void write( const sc_dt::sc_logic& );


    // other methods

    operator const sc_dt::sc_logic& () const
	{ return read(); }


    veri_signal<sc_dt::sc_logic>& operator = ( const sc_dt::sc_logic& a )
	{ write( a ); return *this; }

    veri_signal<sc_dt::sc_logic>& operator = (const veri_signal<sc_dt::sc_logic>& a)
	{ write( a.read() ); return *this; }

    virtual void print( ::std::ostream& = ::std::cout ) const;
    virtual void dump( ::std::ostream& = ::std::cout ) const;

    virtual const char* kind() const
        { return "veri_signal"; }

protected:

    mutable sc_event* m_change_event_p;  // value change event if present.
    sc_dt::sc_logic   m_cur_val;         // current value of object.
    sc_dt::uint64     m_delta;           // delta of last event
    mutable sc_event* m_negedge_event_p; // negative edge event if present.
    sc_port_base*     m_output;          // used for static design rule checking
    mutable sc_event* m_posedge_event_p; // positive edge event if present.
    sc_object*        m_writer;          // process writing this object's value.

private:

    // disabled
    veri_signal( const veri_signal<sc_dt::sc_logic>& );
};


// IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII

inline
void
veri_signal<sc_dt::sc_logic>::register_port( sc_port_base& port_,
				    const char* if_typename_ )
{
    if ( sc_get_curr_simcontext()->write_check() )
    {
	std::string nm( if_typename_ );
	if( nm == typeid( sc_signal_inout_if<sc_dt::sc_logic> ).name() ) {
	    // an out or inout port; only one can be connected
	    if( m_output != 0 ) {
		veri_signal_invalid_writer( this, m_output, &port_ );
	    }
	    m_output = &port_;
	}
    }
}


// write the new value

inline
void
veri_signal<sc_dt::sc_logic>::write( const sc_dt::sc_logic& value_ )
{
    sc_object* writer = sc_get_curr_simcontext()->get_current_writer();
    if( m_writer == 0 ) {
	m_writer = writer;
    } else if( m_writer != writer ) {
	veri_signal_invalid_writer( this, m_writer, writer );
    }

    m_cur_val = value_;
}


inline
void
veri_signal<sc_dt::sc_logic>::print( ::std::ostream& os ) const
{
    os << m_cur_val;
}

inline
void
veri_signal<sc_dt::sc_logic>::dump( ::std::ostream& os ) const
{
    os << "     name = " << name() << ::std::endl;
    os << "    value = " << m_cur_val << ::std::endl;
}


// ----------------------------------------------------------------------------

template <class T>
inline
::std::ostream&
operator << ( ::std::ostream& os, const veri_signal<T>& a )
{
    return ( os << a.read() );
}

} // namespace sc_core

#endif
