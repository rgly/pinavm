#include <vector>

#include "sc_port.h" 

namespace sc_core {

class sc_bind_elem;
class sc_bind_ef;

struct sc_bind_info
{
    // constructor
    explicit sc_bind_info( int max_size_, 
	sc_port_policy policy_=SC_ONE_OR_MORE_BOUND );

    // destructor
    ~sc_bind_info();

    int            max_size() const;
    sc_port_policy policy() const; 
    int            size() const;

    int                        m_max_size;
    sc_port_policy             m_policy;
    std::vector<sc_bind_elem*> vec;
    bool                       has_parent;
    int                        last_add;
    bool                       is_leaf;
    bool                       complete;

    std::vector<sc_bind_ef*>   thread_vec;
    std::vector<sc_bind_ef*>   method_vec;
};

}
