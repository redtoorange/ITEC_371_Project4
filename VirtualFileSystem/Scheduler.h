/*
 *	Andrew McGuiness
 *	ITEC 371 - Project 3
 *	4/1/2018
*/

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <queue>

#include "ProgramFile.h"
#include "Process.h"

/*!
*	\brief A Scheduler manages all jobs on the system.  Jobs are either running, waiting or finished.  The scheduler
*	will only run a single process at a time.  The process will run for a number of ticks equal to the burst time.  If
*	the job finishes before the burst, it will be moved to the finished queue and a new job/burst is started.  If the
*	job is not finished within the burst, it is pushed onto the end of the running queue and a new job is pulled off.  
*	If a job needs to seek IO while running, it will be taken off the queue entirely until it has finished it's IO.
*/
class Scheduler
{
public:
	//! Create a new Scheduler.  The default burst and memory are invalid, must be set by user.
	Scheduler(int mem = -1, int burst = -1);

	//! Run the system until all the jobs on the queues are finished.
	void run();
	void checkForWakingIO();

	//! Advance the system forward 1 time unit.
	void step( int amount );

	//! Set the amount of memoru the process has to hold processes.
	void setMemory( int amount );
	
	//! Set the amount of time a process gets to run before the process is swapped.
	void setBurst( int amount );

	//! Get the amount of memory the scheduler has to store processes.
	int getMemory();

	//! Return the amount of time a process gets to run for before the process is swapped.
	int getBurst();

	/**! 
	 * \brief Attempt to create a new process from the program and add it to the scheduler.  The must be enough
	 * unused memory in the scheduler to add the program.
	*/
	void addProcess( ProgramFile* program );

	
	bool acquireResources( int amount );

private:
	//! Process all jobs currently doing IO, they all advance their IO timer by 1
	void tickIOJobs();

	//! Tick the entire system.  This advances the system time by 1, processes all IO and runs the current process.
	void tickSystem();

	//! Called when a job has finished it's work
	void finishProcess(const std::shared_ptr<Process>& process);

	//! Print spec formatted output for the running jobs
	void printRunningQueue();

	//! Print spec formatted output for job waiting on IO
	void printWaitingQueue();

	//! Print spec formatted output for finished jobs
	void printFinishedQueue();

	//! Print spec formatted output for the current job
	void printCurrentJob();

	//! Print spec formatted output for a "System Update"
	void printSystemUpdate();

	//! Queue that contains all jobs that are running but waiting on the scheduler
	std::deque<std::shared_ptr<Process>> runningJobs;

	//! Queue that contains all jobs that are waiting on the system for IO
	std::deque<std::shared_ptr<Process>> waitingOnIOJobs;

	//! Queue that contains all jobs this scheduler has completed
	std::deque<std::shared_ptr<Process>> finishedJobs;

	//! The currently running process
	std::shared_ptr<Process> currentProcess = nullptr;

	//! How much memory the system has total
	int memoryLimit;

	//! How much memory is in use
	int memoryUsage = 0;

	//! How much time is remaining in the current process' burst
	int remainingBurst;

	//! How much time a process gets per-burst
	int burstTime;

	//! The current system time
	int currentTime = 0;



	int calculateFreeable();
	void freeMemory( int amount );
};
#endif