/*
 *	Andrew McGuiness
 *	ITEC 371 - Project 3
 *	4/1/2018
*/

#ifndef PROCESS_H
#define PROCESS_H

#include "ProgramFile.h"

class Scheduler;

/*!
*	\brief A Process is created by the Scheduler each time a program is started.  The process tracks the program
*	instance's progress.  It tracks the number of units the process has been run, when it started, it's IO data
*	and can detect if the job is done.
*/
class Process
{
public:
	//! Create a new process from the ProgramFile.  Load the metadata of the program file and set process data
	Process( ProgramFile* program, int timestarted );
	
	//! Get the program's memory requirement
	int getMemoryRequired();
	
	//! Get the program's time requirement
	int getTimeRequired();
	
	//! Determine how much longer this process needs to run in main
	int getTimeRemaining();
	
	//! Determine how much time is left on IO
	int getRemainingIOTime();

	//! Is it the right time for this process to start doing IO?
	bool shouldSleepForIO();
	
	//! Has this process completed it's IO work?
	bool shouldWakeFromIO();

	//! Make this process increment is main work
	void tickMain( int currentTime, Scheduler* scheduler );
	
	//! Make this process increment it's IO work
	void tickIO();

	//! Has this process completed the required time units?
	bool isFinished();

	//! Get a formatted string: <name> <time left> <memory usage>
	std::string getData();
	
	//! Get a formatted string: <name> <required Time> <time to complete>
	std::string getFinishedData();

	//! Get the name of the Program that this Process is managing
	std::string getName();


	//! Code added for Project 4
	int shiftToVM();

	bool isInVM();

private:
	std::string name;

	int unitsRun;

	int memoryRequired;
	int timeRequired;

	int timeToIO;
	int amountOfIO;

	int timeStarted;
	int timeFinished;

	//! Code added for Project 4
	bool inVM;
	bool hasResources;
	int vmTicks;

};
#endif