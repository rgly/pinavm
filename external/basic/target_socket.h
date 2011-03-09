#ifndef BASIC_TARGET_SOCKET_H
#define BASIC_TARGET_SOCKET_H

#ifndef BASIC_H
#error include "basic.h"
#endif

namespace basic {

   typedef tlm::tlm_target_socket<CHAR_BIT * sizeof(data_t),
         tlm::tlm_base_protocol_types> compatible_socket;

   template <typename MODULE, bool MULTIPORT = false>
   class target_socket :
         public tlm::tlm_target_socket<CHAR_BIT * sizeof(data_t),
	        tlm::tlm_base_protocol_types, MULTIPORT?0:1>,
         public tlm::tlm_fw_transport_if<tlm::tlm_base_protocol_types>
   {
      typedef tlm::tlm_target_socket<CHAR_BIT * sizeof(data_t),
	     tlm::tlm_base_protocol_types, MULTIPORT?0:1> base_type;
      typedef tlm::tlm_fw_transport_if<tlm::tlm_base_protocol_types> fw_if_type;

      public:

      target_socket() :
            base_type(sc_core::sc_gen_unique_name(kind()))
      {
         init();
      }

      explicit target_socket(const char* name) :
            base_type(name)
      {
         init();
      }

      virtual const char* kind() const {
         return "basic::target_socket";
      }

      bool get_direct_mem_ptr(tlm::tlm_generic_payload&, tlm::tlm_dmi&) {
         std::cerr << "get_direct_mem_ptr not implemented" << std::endl;
         abort();
      }

      unsigned int transport_dbg(tlm::tlm_generic_payload&) {
         std::cerr << "transport_dbg not implemented" << std::endl;
         abort();
      }

      tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload&,
            tlm::tlm_phase&, sc_core::sc_time&) {
         std::cerr << "nb_transport_fw not implemented" << std::endl;
         abort();
      }

      private:

      void b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& t) {
         (void) t;
         addr_t addr = static_cast<addr_t>(trans.get_address());
         data_t& data = *(reinterpret_cast<data_t*>(trans.get_data_ptr()));

         switch(trans.get_command()) {
            case tlm::TLM_READ_COMMAND:
               trans.set_response_status(m_mod->read(addr, data));
               break;
            case tlm::TLM_WRITE_COMMAND:
               trans.set_response_status(m_mod->write(addr, data));
               break;
            case tlm::TLM_IGNORE_COMMAND:
               break;
            default:
               trans.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
         }
      }

      void init() {
         // we'll receive transactions ourselves ...
         this->bind(*(static_cast<fw_if_type*>(this)));
	 // ... but we'll need to call read/write in the parent module.
         m_mod = dynamic_cast<MODULE*>(this->get_parent_object());
         if(!m_mod) {
            std::cerr << this->name() << ": no parent" << std::endl;
            abort();
         }
      }

      MODULE* m_mod;
   };

}

#endif
