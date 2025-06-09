/*
 * sc_simcontext.h
 *
 *  Created on: Sep 23, 2020
 *      Author: dwd
 */

#pragma once

#include <functional>
#include <vector>
#include <set>
#include <algorithm> // find_if
#include <assert.h>
#include <tlm_core/tlm_2/tlm_generic_payload/tlm_gp.h>
#include <sysc/kernel/sc_time.h>
#include <klee_conf.h>
#include <limits>


#include <iostream>

namespace sc_core
{

	typedef int PID;
	typedef void Process(void);
	typedef std::vector<PID> EventNotify(void);
	typedef std::vector<PID> SignalChange(void);
	typedef int EventID;
	typedef int SignalID;

	class Simcontext
	{
		std::vector<std::function<Process>> processes;
		std::vector<std::function<EventNotify>> events;
		std::vector<std::function<SignalChange>> signals;

		std::set<std::pair<sc_time, PID>> wakelist;
		std::set<std::pair<sc_time, EventID>> notifications;

		// Delta
		std::set<SignalID> signalChanges;
		std::set<PID> deltaProcesses;
		std::set<EventID> deltaEvents;

		sc_time global_time;

		PID invalidProcess = -1;
		PID activeProcess = invalidProcess;

		PID recentProcess;

	public:

		static Simcontext& get() {
			static Simcontext context;
			return context;
		}

		static bool isInvalidProcess(PID pid) { return pid < 0 || pid >= get().processes.size(); }
		static PID getInvalidProcess() { return get().invalidProcess; }
		static PID getActiveProcess() { return get().activeProcess; }

		sc_time getGlobalTime()
		{
			return global_time;
		}

		PID registerThread(std::function<Process> t) {
			processes.push_back(t);
			recentProcess = processes.size()-1;
			wakelist.insert({sc_time(0,SC_MS), recentProcess});
			return recentProcess;
		}

		PID registerMethod(std::function<Process> m) {
			processes.push_back(m);
			recentProcess = processes.size()-1;
			wakelist.insert({sc_time(0,SC_MS),recentProcess});
			return recentProcess;
		}

		void addWaketime(PID process, const sc_core::sc_time& time) {
			assert(!isInvalidProcess(process) && "add wake time with invalid process id!");
			if(time == global_time) {
				deltaProcesses.insert(process);
			} else {
				wakelist.insert({time,process});
			}
		}

		void addWaketime(const sc_core::sc_time& time)
		{
			addWaketime(activeProcess, time);
		}

		EventID addEventNotify(const std::function<EventNotify>& notify, const sc_time& time) {
			events.push_back(notify);
			EventID event = events.size()-1;
			addEventNotify(event, time);
			return event;
		}

		void addEventNotify(EventID event, const sc_time& time) {
			auto it = std::find_if(notifications.cbegin(), notifications.cend(),
								   [&](const std::pair<sc_time,EventID>& val) -> bool {
									   return val.second == event;
								   });
			if(time == global_time) {
				deltaEvents.insert(event);
				if(it != notifications.cend()) {
					notifications.erase(it);
				}
			} else if(std::find(deltaEvents.cbegin(),deltaEvents.cend(),event) == deltaEvents.cend()){
				if(it == notifications.end()) {
					notifications.insert({time, event});
				} else if (it->first > time) {
					if(it!=notifications.cend()) {
						notifications.erase(it);
					}
					notifications.insert({time,event});
				}
			}
		}

		SignalID addSignalChange(const std::function<SignalChange>& change) {
			signals.push_back(change);
			SignalID signal = signals.size()-1;
			addSignalChange(signal);
			return signal;
		}

		void addSignalChange(SignalID signal) {
			signalChanges.insert(signal);
		}

		void removeWaketime(EventID event) {
			auto notifications_it = std::find_if(notifications.cbegin(), notifications.cend(),
									 [&](const std::pair<sc_time,EventID>& val) -> bool {
				return val.second == event;
			});
			if(notifications_it != notifications.cend()) {
				notifications.erase(notifications_it);
			}
			auto delta_it = std::find(deltaEvents.cbegin(), deltaEvents.cend(), event);
			if(delta_it != deltaEvents.cend()) {
				deltaEvents.erase(delta_it);
			}
		}

		void removeWaketimeRecentProcess() {
			auto it = std::find_if(wakelist.cbegin(), wakelist.cend(),
								   [&](const std::pair<sc_time,PID>& val) -> bool {
				return val.second == recentProcess;
			});
			if(it!=wakelist.cend()) {
				wakelist.erase(it);
			}

		}

		bool hasNextStep() {
			return !(wakelist.empty() && notifications.empty());
		}

		void runNextStep()
		{
			if(!hasNextStep())
			{
				INFO(std::cout << "[simcontext] No waiting threads" << std::endl);
				return;
			}

			assert((nextGlobalTime() > global_time || nextGlobalTime() == SC_ZERO_TIME) && "Next wake event lies in the past!");
			global_time = nextGlobalTime();

//			INFO(std::cout << "\t >>>Running processes at " << global_time.to_string() << std::endl);

			while(hasExecutableProcesses()) {
				evaluate();
//				INFO(std::cout << "\t\t\t [simcontext] running signal updates" << std::endl);
				update();
//				INFO(std::cout << "\t\t\t [simcontext] running delta phase" << std::endl);
				delta();
			}

//			INFO(std::cout <<"\t <<<End of minikernel step" << std::endl);
			activeProcess = invalidProcess;
		}

		void printInfo()
		{
			INFO(std::cout << "                        processes: " << processes.size() << std::endl);
		}

		void initialize() {
			recentProcess = invalidProcess;
			update();
			delta();
		}

	private:
		Simcontext(){};

		sc_time nextGlobalTime() {
			if(wakelist.empty() || wakelist.cbegin()->first < notifications.cbegin()->first) {
				return notifications.cbegin()->first;
			}
			return wakelist.cbegin()->first;
		}

		bool hasExecutableProcesses() {
			return (!wakelist.empty() && wakelist.cbegin()->first == global_time) ||
				   (!notifications.empty() && notifications.cbegin()->first == global_time);
		}

		void evaluate() {
			while(hasExecutableProcesses())
			{
				if(!wakelist.empty() && wakelist.cbegin()->first == global_time) {
					auto top = wakelist.cbegin();
					activeProcess = top->second;
					wakelist.erase(top);
					if(!isInvalidProcess(activeProcess)) {
						processes[activeProcess]();
					}
				}
				else if(!notifications.empty() && notifications.cbegin()->first == global_time) {
					auto top = notifications.cbegin();
					std::vector<PID> eventProcesses = events[top->second]();
					notifications.erase(top);
					for(auto pid : eventProcesses) {
						activeProcess = pid;
						if(!isInvalidProcess(activeProcess)) {
							processes[activeProcess]();
						}
					}
				}
			}
		}

		void update() {
			for(auto sid : signalChanges) {
				std::vector<PID> signalProcesses = signals[sid]();
				deltaProcesses.insert(signalProcesses.begin(), signalProcesses.end());
			}
			signalChanges.clear();
		}

		void delta() {
			for(auto eid : deltaEvents) {
				notifications.insert({global_time, eid});
			}
			deltaEvents.clear();
			for(auto pid : deltaProcesses) {
				wakelist.insert({global_time,pid});
			}
			deltaProcesses.clear();
		}
	};
};


