#include "sysc/kernel/sc_process.h"
#include "sysc/kernel/sc_cthread_process.h"
#include "sysc/kernel/sc_method_process.h"
#include "sysc/kernel/sc_process_handle.h"

namespace sc_core {

// ----------------------------------------------------------------------------
//  CLASS : sc_process_table
//
//  Container class that keeps track of all method processes,
//  thread processes, and cthread processes.
// ----------------------------------------------------------------------------

class sc_process_table
{
  public:

    sc_process_table();
    ~sc_process_table();
    void push_front( sc_method_handle );
    void push_front( sc_thread_handle );
    void push_front( sc_cthread_handle );
    sc_cthread_handle remove( sc_cthread_handle );
    sc_method_handle remove( sc_method_handle );
    sc_thread_handle remove( sc_thread_handle );


    inline sc_cthread_handle cthread_q_head()
    {
	    return m_cthread_q;
    }

    inline sc_method_handle method_q_head()
    {
	    return m_method_q;
    }

    inline sc_thread_handle thread_q_head()
    {
	    return m_thread_q;
    }
    
  private:

    sc_cthread_handle m_cthread_q; // Queue of existing cthread processes.
    sc_method_handle  m_method_q;  // Queue of existing method processes.
    sc_thread_handle  m_thread_q;  // Queue of existing thread processes.
};

}
