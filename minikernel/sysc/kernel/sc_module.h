#pragma once

#include <sysc/kernel/sc_time.h>
#include <sysc/kernel/sc_module_name.h>
#include <sysc/kernel/sc_simcontext.h>
#include <sysc/kernel/sc_sensitive.h>

#include <map>

#define SC_HAS_PROCESS( user_module_name )                                      \
	typedef user_module_name SC_CURRENT_USER_MODULE

#define declare_method_process(handle, name, host_tag, func) { \
	register_method_process(func); \
}

#define register_method_process(fun) { \
	PID process_id = sc_core::Simcontext::get().registerMethod(std::bind(&SC_CURRENT_USER_MODULE::fun, this)); \
	this->sensitive.setProcess(process_id);                              \
}

#define SC_MODULE(user_module_name)                                           \
    struct user_module_name : ::sc_core::sc_module

#define SC_METHOD(fun)    \
	register_method_process(fun);

#define SC_THREAD(fun) \
	PID process_id = sc_core::Simcontext::get().registerThread(std::bind(&SC_CURRENT_USER_MODULE::fun, this)); \
	this->sensitive.setProcess(process_id);                                                                    \


#define SC_CTHREAD(fun, edge) \
	this->sensitive.setProcess(sc_core::Simcontext::get().registerThread(std::bind(&SC_CURRENT_USER_MODULE::fun, this)));                                                                    \
	this->sensitive << edge;     \
	dont_initialize();\

#define SC_CTOR(user_module_name)                                             \
	typedef user_module_name SC_CURRENT_USER_MODULE;                          \
	user_module_name( ::sc_core::sc_module_name )


namespace sc_core{

	class sc_module
	{
		sc_module_name m_name;
	protected:
		sc_sensitive     sensitive;
		const sc_in<bool>* reset_port;
		bool reset_active_val;
	public:
		explicit sc_module(sc_module_name name) : m_name(name), sensitive(this) {};
		sc_module() : m_name(""), sensitive(this) {};

		const char* name() const
		{ return (const char*) m_name; }
	protected:
		void dont_initialize() {
			Simcontext::get().removeWaketimeRecentProcess();
		}

		void wait() {
			// fairly certain that processes would get woken up anyways and this is more about interruption
		}

		void reset_signal_is( const sc_in<bool>& port, bool level ) {
			reset_port = &port;
			reset_active_val = level;
		}
	};

}
