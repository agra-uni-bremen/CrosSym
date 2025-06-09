#include "sc_event.h"
#include "sc_simcontext.h"
#include <klee_conf.h>

namespace sc_core
{
	// gets all processes !and empties the waiting list!
	std::vector<PID> sc_event::getProcesses() {
		std::vector<PID> all(staticProcesses.size()+waitingProcesses.size());
		all = std::move(waitingProcesses);
		std::copy(staticProcesses.begin(), staticProcesses.end(), std::back_inserter(all));
		return all;
	}

	void sc_event::notify(const sc_time& time) {
		if(eventID >= 0) {
			Simcontext::get().addEventNotify(eventID, Simcontext::get().getGlobalTime() + time);
		} else {
			eventID = Simcontext::get().addEventNotify(std::bind(&sc_event::getProcesses, this),
													   Simcontext::get().getGlobalTime() + time);
		}
	};

	void sc_event::notify() {
		notify(SC_ZERO_TIME);
	}

	void sc_event::cancel() {
		INFO(std::cout << "[event] event cancel" << std::endl);
		assert(eventID >= 0 && "cancel on unwaited event");
		Simcontext::get().removeWaketime(eventID);
	};

	void sc_event::addProcess(PID process_id) {
		std::cout << "\t[event] register process " << process_id << std::endl;
		staticProcesses.push_back(process_id);
	}

/*
sc_time sc_event::getWaketime()
{
	return time_to_wake;
}
*/

	void wait(sc_event& event)
	{
		event.waitingProcesses.push_back(Simcontext::getActiveProcess());
	}

	void wait(const sc_time& time)
	{
		Simcontext::get().addWaketime(Simcontext::get().getGlobalTime() + time);
	}
}
