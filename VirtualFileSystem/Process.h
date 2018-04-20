/*
 *	Andrew McGuiness
 *	ITEC 371 - Project 4
 *	4/19/2018
*/

#ifndef PROCESS_H
#define PROCESS_H

#include "ProgramFile.h"

class Scheduler;

/**
*	\brief A Process is created by the Scheduler each time a program is started.  The process tracks the program
*	instance's progress.  It tracks the number of units the process has been run, when it started, it's IO data
*	and can detect if the job is done.
*/
class Process
{
public:
	/**
	 *	\brief Create a new process from the ProgramFile.  Load the metadata of the program file and set process data.
	 *	
	 *	\param program The ProgramFile that this process represents
	 *	\param timestarted The system time that this job was started at.
	 *	
	 *	Create a new process that represents a job running on the system.  The process is spawned
	 *	from a source program file and should be added to a Scheduler.  The Scheduler will allocate
	 *	time units, ticks, to the process and allow it to run.
	 */
	Process( ProgramFile* program, int timestarted );
	
	/**
	 *	\brief Get the program's memory requirement
	 *	
	 *	\return how much memory this process needs
	 */
	int getMemoryRequired();
	
	/**
	 *	\brief Get the program's time requirement
	 *	
	 *	\return how long this process needs to run in main
	 */
	int getTimeRequired();
	
	/**
	 *	\brief Determine how much longer this process needs to run in main.
	 *	
	 *	\return number of ticks remaining for this process, does not include IO ticks.
	 */
	int getTimeRemaining();
	
	/**
	 *	\brief Return the amount of ticks this process has left before completing it's IO.
	 *	
	 *	\return ticks remaining
	 */
	int getRemainingIOTime();

	/**
	 *	\brief Is it the right time for this process to start doing IO?
	 *	
	 *	Processes needs to sleep at specific times to perform IO.  When they hit that time, they should
	 *	be shifted into a different queue to run on the side.
	 *	
	 *	\return True if this is the tick when the process needs to sleep for IO
	 */
	bool shouldSleepForIO();
	
	/**
	 *	\brief Has this process completed it's IO work?
	 *	
	 *	When a job has 0 IO ticks remaining, it should be moved back to the running queue.
	 *	
	 *	\return true if this process has finished it's IO and should move back to running.
	 */
	bool shouldWakeFromIO();

	/**
	 *	\brief Make this process increment is main work
	 *	
	 *	\param currentTime  The current system time
	 *	\param scheduler	The scheduler that handles all processes
	 *	
	 *	When a job is the current job, it's main will be ticked during it's burst.  If the job is in VM, it will
	 *	attempt to acquire memory from the scheduler.  If it can, it will take 2 ticks to load and then it will tick
	 *	it's time requirements down.
	 *	
	 *	If a process cannot get memory from the scheduler, it will just sleep and wait until the next tick.  The
	 *	idea is that if something wakes from IO and shifts to the job queue, it can be freed next tick.
	 *	
	 *	If the job is not in VM, it will just tick as normal.
	 */
	void tickMain( int currentTime, Scheduler* scheduler );
	
	/**
	 *	\brief Make this process increment it's IO work
	 *	
	 *	The process has performed a tick in it's IO state.
	 */
	void tickIO();

	/**
	 *	\brief Has this process completed the required time units?
	 *	
	 *	\return True if the job has finished running all the required cycles.
	 */
	bool isFinished();

	/**
	 *	\brief Create a new process from the ProgramFile.  Load the metadata of the program file and set process data.
	 *	
	 *	\return Spec formatted string indicating the process's resource usage
	 */
	std::string getData();
	
	/**
	 *	\brief Get a formatted string: <name> <required Time> <time to complete>.
	 *	
	 *	\return Spec formatted string for a finished process
	 */
	std::string getFinishedData();

	/**
	 *	\brief Get the name of the Program that this Process is managing
	 *	
	 *	\return string name of this process's program
	 */
	std::string getName();

	/**
	 *	\brief Shift this process into virtual memory.
	 *	
	 *	Move this process into the virtual memory.  If the process is already there, an exception is thrown.
	 *	
	 *	\return amount of memory freed by this process
	 */
	int shiftToVM();

	/**
	 *	\brief Is this process currently in Virtual Memory?
	 *	
	 *	\return true if this process is currently in VM
	 */
	bool isInVM();

private:
	//! 
	std::string name;

	//! 
	int unitsRun;

	//! 
	int memoryRequired;
	//! 
	int timeRequired;

	//! 
	int timeToIO;
	//! 
	int amountOfIO;

	//! 
	int timeStarted;
	//! 
	int timeFinished;

	//! 
	bool inVM;
	//! 
	bool hasResources;
	//! 
	int vmTicks;

};
#endif