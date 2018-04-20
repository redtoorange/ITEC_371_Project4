/*
 *	Andrew McGuiness
 *	ITEC 371 - Project 4
 *	4/19/2018
*/

#include "Process.h"
#include "Scheduler.h"
#include <sstream>
#include <iostream>

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
Process::Process(ProgramFile* program, int timestarted) 
	: unitsRun(0), timeStarted(timestarted), inVM(false),  hasResources(true), vmTicks(0)
{
	// Populate the process data from the original ProgramFile
	memoryRequired = program->getMemoryRequirements();
	timeRequired = program->getTimeRequirements();

	timeToIO = program->getTimeToDoIO();
	amountOfIO = program->getAmoutOfIO();

	name = program->getFileName();
}

/**
 *	\brief Has this process completed the required time units?
 *	
 *	\return True if the job has finished running all the required cycles.
 */
bool Process::isFinished()
{
	return unitsRun == timeRequired;
}

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
void Process::tickMain( int currentTime, Scheduler* scheduler )
{
	// Try to acquire resources
	if( inVM && !hasResources )
	{
		// Try to get the resources for the job
		hasResources = scheduler->acquireResources(memoryRequired);
	}

	// The proc is in VM, memory was freed, we need to tick the job twice to move out of VM
	if( inVM && hasResources )
	{
		// Tick VM
		vmTicks++;

		// Ticked in VM twice
		if( vmTicks == 2)
		{
			// Pop out of VM
			inVM = false;
			vmTicks = 0;
		}
	}

	// The proc is NOT in VM and has memory, so tick as normal
	else if(!inVM && hasResources )
	{
		// Tick the process's main time
		unitsRun++;

		// Check if the process has finished and set the finish time
		if( isFinished() )
			timeFinished = currentTime + 1;	// add 1 since it technically finishes as the end of the tick
	}

	// No process could be shifted out of memory (perhaps too much IO).
	else
	{
		std::cout << "Proc in VM failed to acquire resources.  Will try again in 1 tick.\n";
	}
}

/**
 *	\brief Make this process increment it's IO work
 *	
 *	The process has performed a tick in it's IO state.
 */
void Process::tickIO()
{
	amountOfIO--;
}

/**
 *	\brief Is it the right time for this process to start doing IO?
 *	
 *	Processes needs to sleep at specific times to perform IO.  When they hit that time, they should
 *	be shifted into a different queue to run on the side.
 *	
 *	\return True if this is the tick when the process needs to sleep for IO
 */
bool Process::shouldSleepForIO()
{
	return (unitsRun == timeToIO) && (amountOfIO > 0);
}

/**
 *	\brief Has this process completed it's IO work?
 *	
 *	When a job has 0 IO ticks remaining, it should be moved back to the running queue.
 *	
 *	\return true if this process has finished it's IO and should move back to running.
 */
bool Process::shouldWakeFromIO()
{
	return amountOfIO == 0;
}

/**
 *	\brief Get the program's memory requirement
 *	
 *	\return how much memory this process needs
 */
int Process::getMemoryRequired()
{
	return memoryRequired;
}

/**
 *	\brief Get the program's time requirement
 *	
 *	\return how long this process needs to run in main
 */
int Process::getTimeRequired()
{
	return timeRequired;
}

/**
 *	\brief Determine how much longer this process needs to run in main.
 *	
 *	\return number of ticks remaining for this process, does not include IO ticks.
 */
int Process::getTimeRemaining()
{
	return timeRequired - unitsRun;
}

/**
 *	\brief Create a new process from the ProgramFile.  Load the metadata of the program file and set process data.
 *	
 *	\return Spec formatted string indicating the process's resource usage
 */
std::string Process::getData()
{
	// Create a string string for faster concats
	std::stringstream ss;

	// Build the string based on spec
	ss << name << " has " << getTimeRemaining() << " time left and is using " << getMemoryRequired();

	// Pluralize if using more than one resource
	std::string r = (getMemoryRequired() > 1) ? "resources" : "resource";

	// Change the message based on process location
	if( inVM )
		ss << " " << r << " on disk.";
	else
		ss << " memory " << r << ".";

	// Send it back
	return ss.str();
}

/**
 *	\brief Return the amount of ticks this process has left before completing it's IO.
 *	
 *	\return ticks remaining
 */
int Process::getRemainingIOTime()
{
	return amountOfIO;
}

/**
 *	\brief Get a formatted string: <name> <required Time> <time to complete>.
 *	
 *	\return Spec formatted string for a finished process
 */
std::string Process::getFinishedData()
{
	// Create a string stream for faster concat
	std::stringstream ss;

	// Build the string accorind to spec
	ss << name << " " << getTimeRequired() << " " << (timeFinished - timeStarted);

	// Send it back
	return ss.str();
}

/**
 *	\brief Get the name of the Program that this Process is managing
 *	
 *	\return string name of this process's program
 */
std::string Process::getName()
{
	return name;
}

/**
 *	\brief Shift this process into virtual memory.
 *	
 *	Move this process into the virtual memory.  If the process is already there, an exception is thrown.
 *	
 *	\return amount of memory freed by this process
 */
int Process::shiftToVM()
{
	// Already in VM, this is fatal error
	if( inVM ) throw std::exception("Process is already in VM");

	// Flag as in VM and disable resources
	inVM = true;
	hasResources = false;

	// Return the resources freed by this process
	return memoryRequired;
}

/**
 *	\brief Is this process currently in Virtual Memory?
 *	
 *	\return true if this process is currently in VM
 */
bool Process::isInVM()
{
	return inVM;
}