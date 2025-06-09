#pragma once

#include <sysc/communication/sc_signal.h>
#include <sysc/communication/sc_signal_ports.h>

namespace sc_core {
	class sc_module;

	class sc_sensitive {

		PID		m_process_id;

	public:
		explicit sc_sensitive(sc_module*);
		~sc_sensitive();

		void setProcess(PID p_id);

		sc_sensitive& operator << (sc_event&);
		sc_sensitive& operator << (sc_signal<bool>&);
		sc_sensitive& operator << (sc_in<bool>&);
		sc_sensitive& operator << (sc_in<uint32_t>&);
	};
}
