/*
 *	Andrew McGuiness
 *	ITEC 371 - Project 4
 *	4/19/2018
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
	/**
	 *	\brief Create a new Scheduler with the specified burst and memory.  
	 *	
	 *	The default burst and memory are invalid to begin with.  The user must set them by calling
	 *	setMemory and setBurst.  This was done as recommended by the spec.
	 *	
	 *	\param mem Amount of memory that the scheduler can allocate to running processes
	 *	\param burst Amount of time to allocate each process while it's running
	 */
	Scheduler(int mem = -1, int burst = -1);

	/**
	 *	\brief Run the system until all the jobs on all the queues are finished.
	 *	
	 *	The system will run in bursts of time.  A process is pulled from the running queue and 
	 *	ticked the number of times allowed by the burst.  It is then sent to the end of the queue.
	 *	
	 *	If the job needs IO, it is moved off the running queue and into the IO queue where it will run
	 *	in parallel with the rest of the jobs.
	 *	
	 *	If the job is in VM, it will be allocated memory before it can run.
	 */
	void run();

	/**
	 *	\brief Scan the getting IO queue to see if any of the jobs are down with IO.
	 *	
	 *	If a job on the IO queue has finished getting IO, it will be moved to the 
	 *	end of the running queue.  If it is not finished, it will just be kept on 
	 *	the IO queue.
	 */
	void checkForWakingIO();

	/**
	 *	\brief Advance the system forward by a number of time units.
	 *	
	 *	This is similar to calling the run function.  However, instead of running until everything is 
	 *	finished, the system is only ticked a specified number of units.
	 *	
	 *	\amount How many times to tick the system.
	 */
	void step( int amount );

	/**
	 *	\brief Set the amount of memory the scheduler can allocate to jobs.
	 *	
	 *	\param amount How much memory to have
	 */
	void setMemory( int amount );
	
	/**
	 *	\brief Set the amount of time each process is allocated to run in.
	 *	
	 *	\param amount How much time to give a job when it is running.
	 *	
	 *	The system must have a burst time of atleast 2.  This is because a job requires 
	 *	2 ticks to be loaded from VM.  If the burst time is less than 2, jobs will never
	 *	get loaded.
	 */
	void setBurst( int amount );

	/**
	 *	\return The amount of memory that the scheduler has to allocate to jobs.
	 */
	int getMemory();

	/**
	 *	\return The amount of time the scheduler will allocate to a job before it is swapped.
	 */
	int getBurst();

	/**
	 *	\brief Add a ProgramFile to the scheduler's job queue.
	 *	
	 *	\param program A ProgramFile that should be converted into a process and added
	 *	to the scheduler.
	 *	
	 *	The ProgramFile will be checked to ensure that the system has enough memory to run 
	 *	the Program.  If it does, the Program will be converted into a process.  This process
	 *	will be allocated memory.  If there is not enough available, the shceduler will use
	 *	the RMU algorithm to shift a process into VM.
	 */
	void addProcess( ProgramFile* program );

	/**
	 *	\brief Attempt to acquire resources for a process.  This is done by shifting
	 *	jobs into VM which frees their memory.
	 *	
	 *	\param amount How much memory needs to be acquired
	 *	
	 *	There is a chance that the required memory cannot be acquired.  If there are jobs
	 *	getting IO, they are not considered for a VM shift.  Likewise, a job that is currently
	 *	running cannot be moved to VM.
	 *	
	 *	\return True if the memory was acquired (freed from other processes)
	 */
	bool acquireResources( int amount );

private:
	/**
	 *	\brief Tick all the jobs that are waiting on IO.
	 *	
	 *	IO jobs are checked to see if they should wake in a separate function.  This function just
	 *	updates all IO job internal timers.
	 */
	void tickIOJobs();

	/**
	 *	\brief Tick the system clock and run all processes by 1 time unit.
	 *	
	 *	All jobs on the IO queue will be updated and the running job will
	 *	have it's main ticked.  If the current job finishes it will be pushed
	 *	to the finished queue, if it needs IO it will be sent to that queue.  If
	 *	it has exhausted it's burst, it will be pushed to the back of the running
	 *	queue.
	 *	
	 *	If there is no active job, the next one will be pulled from the running queue.  
	 *	If there are no jobs available, only IO is ticked.
	 */
	void tickSystem();

	/**
	 *	\brief A process has finished working, move it to the finished queue and free it's memory.
	 *	
	 *	\param process The process that has finished it's job.
	 */
	void finishProcess(const std::shared_ptr<Process>& process);

	/**
	 *	\brief Print data for each process on the Running job's queue.
	 *	
	 *	The output from this function is based on the Project Spec.
	 */
	void printRunningQueue();

	/**
	 *	\brief Print data for each process on the IO queue.
	 *	
	 *	The output from this function is based on the Project Spec.
	 */
	void printWaitingQueue();

	/**
	 *	\brief Print data for each process on the finished queue.
	 *	
	 *	The output from this function is based on the Project Spec.
	 */
	void printFinishedQueue();

	/**
	 *	\brief	Print information on the currently running process. 
	 */
	void printCurrentJob();

	/**
	 *	\brief Print spec formatted output for a "System Update".
	 */
	void printSystemUpdate();

	/**
	 *	\brief Calculate how much memory can be taken from jobs that are on the running queue.
	 *	
	 *	Only jobs that are currently on the running queue but that are not actually running are
	 *	considered for freeing.  A job that is already in VM is also ignored.
	 *	
	 *	\return How much memory can be freed from the system maximum.
	 */
	int calculateFreeable();

	/**
	 *	\brief Start shifting jobs into VM until we have the minimum amount of
	 *	memory required.
	 *	
	 *	This uses the MRU algorithm to shift jobs into the VM.  This is not the most efficient
	 *	way of doing it, as it might free more memory than needed.
	 *	
	 *	\param amount How much memory was actually freed.  This may be more than was requested.
	 */
	void freeMemory( int amount );

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
	int currentTime;
};
#endif