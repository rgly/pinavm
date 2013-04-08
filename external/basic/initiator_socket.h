// -*- c-basic-offset: 3 -*-
#ifndef BASIC_INITIATOR_SOCKET_H
#define BASIC_INITIATOR_SOCKET_H

#ifndef BASIC_H
#error please, include "basic.h"
#endif

#include "basic_config.h"
#include <vector>

namespace basic {

   template <bool MULTIPORT = false>
   class initiator_socket_base :
      public tlm::tlm_initiator_socket<CHAR_BIT * sizeof(data_t),
				       tlm::tlm_base_protocol_types,
				       MULTIPORT?0:1>,
      private tlm::tlm_bw_transport_if<tlm::tlm_base_protocol_types>
   {
      typedef tlm::tlm_initiator_socket<CHAR_BIT * sizeof(data_t),
	     tlm::tlm_base_protocol_types, MULTIPORT?0:1> base_type;
      typedef tlm::tlm_bw_transport_if<tlm::tlm_base_protocol_types> bw_if_type;

      public:

      initiator_socket_base();
      explicit initiator_socket_base(const char *name);

      virtual ~initiator_socket_base() {
         tlm::tlm_generic_payload* trans;
         while(!container.empty()) {
            trans = container.back();
            container.pop_back();
            delete trans;
         }
      }

      tlm::tlm_response_status read(const addr_t& addr, data_t& data, int port = 0) {
         
          // Testing the bypass
          BASIC_TRACE_DEBUG("[!] CALL THE INITIATOR'S READ FUNCTION [!]\n");
          
          tlm::tlm_generic_payload* trans;
          // allocate the payload
         if(!container.empty()) {
            trans = container.back();
            container.pop_back();
         }
         else {
            trans = new tlm::tlm_generic_payload();
         }

	 // build the payload ...
         trans->set_command(tlm::TLM_READ_COMMAND);
         trans->set_address(addr);

         trans->set_data_ptr(reinterpret_cast<unsigned char*>(&data));
	 // No block transaction => just one piece of data
         trans->set_data_length(sizeof(data_t));
	 // no streaming => streaming_width == data_length
         trans->set_streaming_width(sizeof(data_t));

	 // ... and send it.
         (*this)[port]->b_transport(*trans, time);

         container.push_back(trans);

         return trans->get_response_status();
      }

      tlm::tlm_response_status write(const addr_t& addr, data_t data, int port = 0) {
         
          // Testing the bypass
          BASIC_TRACE_DEBUG("[!] CALL THE INITIATOR'S WRITE FUNCTION [!]\n");
          
          tlm::tlm_generic_payload* trans;

         if(!container.empty()) {
            trans = container.back();
            container.pop_back();
         }
         else {
            trans = new tlm::tlm_generic_payload();
         }

         trans->set_command(tlm::TLM_WRITE_COMMAND);
         trans->set_address(addr);

         trans->set_data_ptr(reinterpret_cast<unsigned char*>(&data));
         trans->set_data_length(sizeof(data_t));
         trans->set_streaming_width(sizeof(data_t));

         (*this)[port]->b_transport(*trans, time);

         container.push_back(trans);

         return trans->get_response_status();
      }

      virtual const char* kind() const {
         return "basic::initiator_socket";
      }

      void invalidate_direct_mem_ptr(sc_dt::uint64, sc_dt::uint64)
      {
         std::cerr << "invalidate_direct_mem_ptr not implemented" << std::endl;
         abort();
      }

      tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload&,
            tlm::tlm_phase&, sc_core::sc_time&)
      {
         std::cerr << "nb_transport_bw not implemented" << std::endl;
         abort();
      }

      private:

      // container to keep the unused payloads (avoids calling new too often)
      std::vector<tlm::tlm_generic_payload*> container;

      // zero time, but allocated once and for all for performance reasons.
      sc_core::sc_time time;

	  
      void init() {
	 // we're not actually using the backward interface,
	 // but we need to bind the sc_export of the socket to something.
         this->bind(*(static_cast<bw_if_type*>(this)));
      }

      virtual void dummy();

   };

   /*!
    * Just a wrapper around initiator_socket_base, to give a
    * consistant interface where everybody has a MODULE template
    * argument.
    */
   template <typename MODULE, bool MULTIPORT = false>
   class initiator_socket : public initiator_socket_base<MULTIPORT>,
		      /* to be able to call protected constructor : */
			    public virtual sc_core::sc_interface {
   public:
      initiator_socket()
	 : initiator_socket_base<MULTIPORT>(), sc_core::sc_interface() { /* */ }

      explicit initiator_socket(const char* name)
	 : initiator_socket_base<MULTIPORT>(name) { /* */ }
   };
}

#endif
