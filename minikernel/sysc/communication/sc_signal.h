#pragma once

#include <sysc/communication/sc_writer_policy.h>
#include <sysc/kernel/sc_event.h>


// TODO writer policy so far not further considered in signals
// only used in simple bus, which cannot be used anyway
// not in verilated models itself, which is current focus
namespace sc_core {
	// T
	template<class T, sc_writer_policy POL=SC_ONE_WRITER>
	class sc_signal
	{
		std::vector<PID> m_processes;
		T m_cur_val;
		T m_new_val;

		SignalID signalID = -1;
	protected:
		typedef sc_signal<T,POL>            this_type;

	public:
		explicit sc_signal(const char* name_) : m_cur_val() {}
		sc_signal() : m_cur_val() {}

		this_type& operator=(const T &a) {
			write(a);
			return *this;
		}

		const T& read() const{
			return m_cur_val;
		}
		void write(const T &a) {
			m_new_val = a;
			if(signalID >= 0) {
				Simcontext::get().addSignalChange(signalID);
			} else {
				signalID = Simcontext::get().addSignalChange(std::bind(&sc_signal::update, this));
			}
		}

		void add_process(PID process_id) {
			if(!Simcontext::isInvalidProcess(process_id)) {
				m_processes.push_back(process_id);
			}
		}
	private:
		std::vector<PID> update() {
			if(m_cur_val == m_new_val) {
				return {};
			}
			m_cur_val = m_new_val;
			m_new_val = T();
			return m_processes;
		}
	};

	// bool
	template<sc_writer_policy POL>
	class sc_signal<bool,POL>
	{
		std::vector<PID> m_processes;
		bool m_cur_val;
		bool m_new_val;
		SignalID signalID = -1;
	protected:
		typedef sc_signal<bool,POL>         this_type;

		sc_event m_posedge_event;
		sc_event m_negedge_event;
		std::vector<sc_event*> port_posedges;

	public:
		explicit sc_signal(const char* name_) : m_cur_val(), m_posedge_event(), m_negedge_event() {}
		sc_signal() : m_cur_val(), m_posedge_event(), m_negedge_event() {}

		sc_event& posedge_event() { return m_posedge_event; }
		sc_event& negedge_event() { return m_negedge_event; }

		const bool& read() const {
			return m_cur_val;
		}
		void write(const bool &a) {
			m_new_val = a;
			if(signalID >= 0) {
				Simcontext::get().addSignalChange(signalID);
			} else {
				signalID = Simcontext::get().addSignalChange(std::bind(&sc_signal::update, this));
			}
		}

		this_type& operator=(const bool &a) {
			write(a);
			return *this;
		}

		void add_process(PID process_id) {
			if(!Simcontext::isInvalidProcess(process_id)) {
				m_processes.push_back(process_id);
			}
		}
		void add_posedge(sc_event *pos) {
			port_posedges.push_back(pos);
		}
	private:
		std::vector<PID> update() {
			if(m_cur_val == m_new_val) {
				return {};
			}
			m_cur_val = m_new_val;
			m_new_val = bool();
			if(m_cur_val) {
				m_posedge_event.notify(sc_time(0, sc_core::SC_NS));
				for (auto p: port_posedges) {
					p->notify(sc_time(0, sc_core::SC_NS));
				}
			}
			else {
				m_negedge_event.notify(sc_time(0, sc_core::SC_NS));
			}
			return m_processes;
		}
	};
}

//#include "sc_signal.tpp"
