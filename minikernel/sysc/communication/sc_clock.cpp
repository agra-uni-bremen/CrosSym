#include <sysc/communication/sc_clock.h>
#include <iostream>

namespace sc_core {

	sc_clock::sc_clock(const char* name_,
					   const sc_time& period_,
					   double         duty_cycle_,
					   const sc_time& start_time_,
					   bool           posedge_first_) :
			base_type(name_),
			m_period(), m_duty_cycle(), m_start_time(), m_posedge_first(),
			m_posedge_time(), m_negedge_time(),
			m_posedge_pid(Simcontext::getInvalidProcess()), m_negedge_pid(Simcontext::getInvalidProcess()) {
		// initialize
		if(period_ == SC_ZERO_TIME) {
			std::cerr << "Increase the period" << std::endl;
			// abbrechen?
		}
		m_period = period_;
		m_posedge_first = posedge_first_;

		if(duty_cycle_ <= 0.0 || duty_cycle_ >= 1.0) {
			m_duty_cycle = 0.5;
		} else {
			m_duty_cycle = duty_cycle_;
		}

		m_negedge_time = m_period * m_duty_cycle;
		m_posedge_time = m_period - m_negedge_time;

		if(m_negedge_time == SC_ZERO_TIME) {
			std::cerr << "Increase the period or increase the duty cycle" << std::endl;
			// abbrechen?
		}
		if(m_posedge_time == SC_ZERO_TIME) {
			std::cerr << "Increase the period or decrease the duty cycle" << std::endl;
			// abbrechen?
		}

//		if(posedge_first_) {
//			write(false);
//		} else {
//			write(true);
//		}

		m_start_time = start_time_;

		// Register Methods for posedge/negedge
		m_posedge_pid = Simcontext::get().registerMethod(std::bind(&sc_clock::posedge_action, this));
		Simcontext::get().removeWaketimeRecentProcess();
		m_next_posedge.addProcess(m_posedge_pid);
		m_negedge_pid = Simcontext::get().registerMethod(std::bind(&sc_clock::negedge_action, this));
		Simcontext::get().removeWaketimeRecentProcess();
		m_next_negedge.addProcess(m_negedge_pid);

		// notify first event
		if(posedge_first_) {
			m_next_posedge.notify(m_start_time);
		} else {
			m_next_negedge.notify(m_start_time);
		}
	}

	sc_clock::~sc_clock() {}

	void sc_clock::posedge_action() {
		m_next_negedge.notify(m_negedge_time);
		write(true);
	}

	void sc_clock::negedge_action() {
		m_next_posedge.notify(m_posedge_time);
		write(false);
	}
}
