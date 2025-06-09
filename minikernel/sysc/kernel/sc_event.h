#pragma once

#include <sysc/kernel/sc_time.h>
#include <sysc/kernel/sc_simcontext.h>
#include <functional>

namespace sc_core
{

/*
 * Is it allowed for an Event to be notified before anyone waits for it?
 *
 */


struct sc_event
{
	std::vector<PID> staticProcesses;
	std::vector<PID> waitingProcesses;

	EventID eventID = -1;

private:
	std::vector<PID> getProcesses();

public:
	//notify for use in benchmark to require a certain reaction
	void notify(const sc_time& time);
	void notify();
	//sc_time getWaketime();
	void cancel();
	void addProcess(PID process_id);
};


void wait(sc_event& event);
void wait(const sc_time& time);

}
