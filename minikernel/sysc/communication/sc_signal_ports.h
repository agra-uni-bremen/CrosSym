#pragma once

#include <sysc/communication/sc_signal.h>
#include <sysc/communication/sc_writer_policy.h>

namespace sc_core {

	template<class T>
	class sc_in
	{
		sc_signal<T,SC_MANY_WRITERS> *m_interface_many; // TODO obv should not actually be two seperate variables...
		sc_signal<T,SC_ONE_WRITER> *m_interface_one;
		std::vector<PID> m_processes;
	public:
		typedef T data_type;

		explicit sc_in( const char* name_ ) : m_interface_many(nullptr), m_interface_one(nullptr) {}
		sc_in() : m_interface_many(nullptr), m_interface_one(nullptr) {}

		void operator()(sc_signal<T,SC_MANY_WRITERS> &interface) {
			m_interface_many = &interface;
			m_interface_one = nullptr;
			for(auto pid : m_processes) {
				m_interface_many->add_process(pid);
			}
		}
		void operator()(sc_signal<T,SC_ONE_WRITER> &interface) {
			m_interface_one = &interface;
			m_interface_many = nullptr;
			for(auto pid : m_processes) {
				m_interface_one->add_process(pid);
			}
		}
		const data_type& read() {
			assert((m_interface_many!=nullptr || m_interface_one != nullptr) && "Port is not bound to signal");
			return m_interface_one != nullptr ? m_interface_one->read() : m_interface_many->read();
		}

		void add_process(PID process_id) {
			if(m_interface_one == nullptr && m_interface_many == nullptr) {
				m_processes.push_back(process_id);
			}
			else if (m_interface_one != nullptr) {
				m_interface_one->add_process(process_id);
			} else {
				m_interface_many->add_process(process_id);
			}
		}
	};

// sc_in<bool>

	template<>
	class sc_in<bool> {
		sc_signal<bool,SC_MANY_WRITERS> *m_interface_many; // TODO obv should not actually be two seperate variables...
		sc_signal<bool,SC_ONE_WRITER> *m_interface_one;
		std::vector<PID> m_processes;

		sc_event port_posedge;
	public:
		typedef bool data_type;

		explicit sc_in( const char* name_ ) : m_interface_many(nullptr), m_interface_one(nullptr), port_posedge() {}
		sc_in() : m_interface_many(nullptr), m_interface_one(nullptr), port_posedge() {}

		void operator()(sc_signal<bool,SC_MANY_WRITERS> &interface) {
			m_interface_many = &interface;
			m_interface_one = nullptr;
			m_interface_many->add_posedge(&port_posedge);
			for(auto pid : m_processes) {
				m_interface_many->add_process(pid);
			}
		}
		void operator()(sc_signal<bool,SC_ONE_WRITER> &interface) {
			m_interface_one = &interface;
			m_interface_many = nullptr;
			m_interface_one->add_posedge(&port_posedge);
			for(auto pid : m_processes) {
				m_interface_one->add_process(pid);
			}
		}
		const data_type& read() const {
			assert((m_interface_many!=nullptr || m_interface_one != nullptr) && "Port is not bound to signal");
			return m_interface_one != nullptr ? m_interface_one->read() : m_interface_many->read();
		}

		void add_process(PID process_id) {
			if(m_interface_one == nullptr && m_interface_many == nullptr) {
				m_processes.push_back(process_id);
			}
			else if (m_interface_one != nullptr) {
				m_interface_one->add_process(process_id);
			} else {
				m_interface_many->add_process(process_id);
			}
		}

		sc_event& pos() {
			return port_posedge;
		}
	};

// sc_out

	template<class T>
	class sc_out {
		sc_signal<T,SC_ONE_WRITER> *m_interface;
	public:
		typedef T data_type;

		explicit sc_out( const char* name_ ) : m_interface(nullptr) {}
		sc_out() : m_interface(nullptr) {}

		void operator()(sc_signal<T,SC_ONE_WRITER> &interface) {
			m_interface = &interface;
		}
		void write(const data_type& value_) {
			assert(m_interface!=nullptr && "Port is not bound to signal");
			m_interface->write(value_);
		}
	};
}

//#include "sc_signal_ports.tpp"
