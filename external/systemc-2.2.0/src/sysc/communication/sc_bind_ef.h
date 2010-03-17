namespace sc_core {

class sc_process_b;
class sc_event_finder;

struct sc_bind_ef
{
    // constructor
    sc_bind_ef( sc_process_b* , sc_event_finder* );

    // destructor
    ~sc_bind_ef();

    sc_process_b* handle;
    sc_event_finder* event_finder;
};

}
