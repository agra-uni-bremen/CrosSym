#include <sysc/kernel/sc_sensitive.h>

namespace sc_core {
	sc_sensitive::sc_sensitive(sc_module *) : m_process_id(Simcontext::getInvalidProcess()) {
	}

	sc_sensitive::~sc_sensitive() {}

	void sc_sensitive::setProcess(PID p_id) {
		m_process_id = p_id;
	}

	sc_sensitive &sc_sensitive::operator<<(sc_event &event) {
		event.addProcess(m_process_id);
		return *this;
	}

	sc_sensitive &sc_sensitive::operator<<(sc_in<bool> &port) {
		port.add_process(m_process_id);
		return *this;
	}

	sc_sensitive &sc_sensitive::operator<<(sc_in<uint32_t> &port) {
		port.add_process(m_process_id);
		return *this;
	}

	sc_sensitive &sc_sensitive::operator<<(sc_signal<bool> &port) {
		port.add_process(m_process_id);
		return *this;
	}
}
