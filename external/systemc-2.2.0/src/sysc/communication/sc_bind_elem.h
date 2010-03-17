namespace sc_core {

class sc_port_base;
class sc_interface;

struct sc_bind_elem
{
    // constructors
    sc_bind_elem();
    explicit sc_bind_elem( sc_interface* interface_ );
    explicit sc_bind_elem( sc_port_base* parent_ );

    sc_interface* iface;
    sc_port_base* parent;
};

}
