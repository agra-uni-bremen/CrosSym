#pragma once

#include <sysc/kernel/sc_time.h>
#include <sysc/communication/sc_signal.h>

namespace sc_core {
	class sc_clock : public sc_signal<bool,SC_ONE_WRITER>
	{
		typedef sc_signal<bool,SC_ONE_WRITER> base_type;
	protected:
		sc_time  m_period;
		double   m_duty_cycle;
		sc_time  m_start_time;
		bool     m_posedge_first;
		sc_time  m_posedge_time;
		sc_time  m_negedge_time;

		PID m_posedge_pid;
		PID m_negedge_pid;

		sc_event m_next_posedge;
		sc_event m_next_negedge;

		void posedge_action();
		void negedge_action();

	public:
		sc_clock( const char* name_,
				  const sc_time& period_,
				  double         duty_cycle_ = 0.5,
				  const sc_time& start_time_ = SC_ZERO_TIME,
				  bool           posedge_first_ = true );
		~sc_clock();
	};
}