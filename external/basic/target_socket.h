// -*- c-basic-offset: 3 -*-
#ifndef BASIC_TARGET_SOCKET_H
#define BASIC_TARGET_SOCKET_H

#ifndef BASIC_H
#error please include "basic.h"
#endif

namespace basic {
   class target_module_base {
   public:
      target_module_base();
      virtual void dummy();
      virtual tlm::tlm_response_status write(const basic::addr_t &a,
					     const basic::data_t &d) = 0;
      
      virtual tlm::tlm_response_status read (const basic::addr_t &a,
					     /* */ basic::data_t &d) = 0;
   };

   typedef tlm::tlm_target_socket<CHAR_BIT * sizeof(data_t),
         tlm::tlm_base_protocol_types> compatible_socket;

   template <bool MULTIPORT = false>
   class target_socket_base :
         public tlm::tlm_target_socket<CHAR_BIT * sizeof(data_t),
	        tlm::tlm_base_protocol_types, MULTIPORT?0:1>,
         public tlm::tlm_fw_transport_if<tlm::tlm_base_protocol_types>
   {
      typedef tlm::tlm_target_socket<CHAR_BIT * sizeof(data_t),
	     tlm::tlm_base_protocol_types, MULTIPORT?0:1> base_type;
      typedef tlm::tlm_fw_transport_if<tlm::tlm_base_protocol_types> fw_if_type;

      public:

      target_socket_base();
      explicit target_socket_base(const char* name);

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

      void init() {
         // we'll receive transactions ourselves ...
         this->bind(*(static_cast<fw_if_type*>(this)));
	 // ... but we'll need to call read/write in the parent module.
	 // => done in derived class target_socket
      }

   };

//#define MODULE basic::target_module_base
   template <typename MODULE, bool MULTIPORT = false>
   class target_socket : public target_socket_base<MULTIPORT>,
			 /* to be able to call protected constructor : */
			 public virtual sc_core::sc_interface {
   public:

      target_socket() 
	 : target_socket_base<MULTIPORT>() { init_parent(); }
      
      target_socket(const char* name)
	 : target_socket_base<MULTIPORT>(name) { init_parent(); }

   private:
      void init_parent() {
         m_mod = dynamic_cast<MODULE*>(this->get_parent_object());
         if(!m_mod) {
            std::cerr << this->name() << ": cannot cast parent" << std::endl;
            std::cerr << "parent name: " << this->get_parent_object()->name() << std::endl;
   
            abort();
         }
      }
   public:
      MODULE *get_parent() {
	 return m_mod;
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

      MODULE* m_mod;
   };
}

#endif
