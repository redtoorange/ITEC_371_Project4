/*
 *	Andrew McGuiness
 *	ITEC 371 - Project 3
 *	4/1/2018
*/

#include "Process.h"
#include "Scheduler.h"
#include <sstream>
#include <iostream>

Process::Process(ProgramFile* program, int timestarted) : timeStarted(timestarted)
{
	// Populate the process data
	memoryRequired = program->getMemoryRequirements();
	timeRequired = program->getTimeRequirements();

	timeToIO = program->getTimeToDoIO();
	amountOfIO = program->getAmoutOfIO();

	name = program->getFileName();

	unitsRun = 0;
	vmTicks = 0;
	hasResources = true;
	inVM = false;
}


bool Process::isFinished()
{
	return unitsRun == timeRequired;
}

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
		//std::cout << "Ticking out of VM\n";

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
		unitsRun++;
		//std::cout << "Run units: " << unitsRun<< "\n";

		if( isFinished() )
			timeFinished = currentTime + 1;
	}

	else
	{
		std::cout << "Proc in VM failed to acquire resources...";
	}
}

void Process::tickIO()
{
	amountOfIO--;
}

bool Process::shouldSleepForIO()
{
	return (unitsRun == timeToIO) && (amountOfIO > 0);
}

bool Process::shouldWakeFromIO()
{
	return amountOfIO == 0;
}

int Process::getMemoryRequired()
{
	return memoryRequired;
}

int Process::getTimeRequired()
{
	return timeRequired;
}

int Process::getTimeRemaining()
{
	return timeRequired - unitsRun;
}


std::string Process::getData()
{
	std::stringstream ss;

	ss << name << " has " << getTimeRemaining() << " time left and is using " << getMemoryRequired();

	const std::string r = (getMemoryRequired() > 1) ? "resources" : "resource";
	if( inVM )
		ss << " " << r << " on disk.";
	else
		ss << " memory " << r << ".";

	return ss.str();
}

int Process::getRemainingIOTime()
{
	return amountOfIO;
}

std::string Process::getFinishedData()
{
	std::stringstream ss;

	ss << name << " " << getTimeRequired() << " " << (timeFinished - timeStarted);

	return ss.str();
}


std::string Process::getName()
{
	return name;
}


int Process::shiftToVM()
{
	inVM = true;
	hasResources = false;

	return memoryRequired;
}


bool Process::isInVM()
{
	return inVM;
}
