/*
 *	Andrew McGuiness
 *	ITEC 371 - Project 3
 *	4/1/2018
*/

#include "Scheduler.h"
#include <ostream>
#include <iostream>


Scheduler::Scheduler(int mem, int burst)
	: memoryLimit(mem), burstTime(burst), currentTime(0)
{
}

void Scheduler::run()
{
	// Ensure that the burst time is atleast 1
	if( burstTime <= 0 )
	{
		std::cout << "Burst Time must be configured and greater than 0 before running.\n";
		return;
	}

	std::cout << "\nAdvancing the system until all jobs finished\n";

	// Tick the system until all jobs done
	while( currentProcess || !runningJobs.empty() || !waitingOnIOJobs.empty())
	{
		tickSystem();
	}

	// Final system update
	printSystemUpdate();
}


void Scheduler::checkForWakingIO()
{
	std::deque<std::shared_ptr<Process>> tempJobs;
	while( !waitingOnIOJobs.empty() )
	{
		auto j = waitingOnIOJobs.front();
		waitingOnIOJobs.pop_front();

		if( j->shouldWakeFromIO() )
		{
			runningJobs.push_back(j);
		}
		else
		{
			tempJobs.push_back(j);
		}
	}
	waitingOnIOJobs = tempJobs;
}

void Scheduler::tickSystem()
{
	// If it's time to swap or the current process has been moved off queue
	if( !currentProcess || remainingBurst == 0)
	{
		// Used when the burst is finished
		if( currentProcess )
			runningJobs.push_back( currentProcess );

		// Grab the next process available
		if( !runningJobs.empty() )
		{
			// Grab the next process
			currentProcess = runningJobs.front();
			runningJobs.pop_front();
			remainingBurst = burstTime;
			
			// Update with system output
			printSystemUpdate();
			std::cout << "Next burst time <" << remainingBurst << ">\n";
		}
		else if(!waitingOnIOJobs.empty())
		{
			std::cout << "No processes to run, waiting on IO.\n";
		}
		else
		{
			std::cout << "All queues are empty.\n";
			return;
		}
	}

	// Find a process
	if( currentProcess )
		currentProcess->tickMain( currentTime, this );

	tickIOJobs();

	// Move the current job
	if (currentProcess && currentProcess->shouldSleepForIO())
	{
		waitingOnIOJobs.push_back( currentProcess );
		currentProcess = nullptr;
	}
	if( currentProcess && currentProcess->isFinished())
	{
		finishProcess( currentProcess );
		currentProcess = nullptr;
	}

	checkForWakingIO();

	currentTime++;
	remainingBurst--;
}

void Scheduler::tickIOJobs()
{
	// Step all IO processes
	std::deque<std::shared_ptr<Process>> tempJobs;
	while( !waitingOnIOJobs.empty() )
	{
		auto j = waitingOnIOJobs.front();
		waitingOnIOJobs.pop_front();

		j->tickIO();
		tempJobs.push_back(j);
	}
	waitingOnIOJobs = tempJobs;
}

void Scheduler::step(int amount)
{
	if( burstTime <= 0 )
	{
		std::cout << "Burst Time must be configured and greater than 0 before running.\n";
		return;
	}

	std::cout << "\nAdvancing the system for " << amount << " units or until all jobs finished\n";

	while( amount > 0 && (currentProcess || !runningJobs.empty() || !waitingOnIOJobs.empty()) )
	{
		tickSystem();
		amount--;
	}

	std::cout << "System state at the end of stepping:\n";
	printSystemUpdate();
}


void Scheduler::printCurrentJob()
{
	if( currentProcess )
		std::cout << "Running job " << currentProcess->getData() << "\n";
	else
		std::cout << "Running job is empty\n";
}

void Scheduler::setMemory(int amount)
{
	if( amount >= 0)
		memoryLimit = amount;
	else
		std::cout << "Error: System cannot have negative memory...\n";
}

void Scheduler::setBurst(int amount)
{
	if( amount >= 0)
	{
		burstTime = amount;
		remainingBurst = burstTime;
	}
	else
		std::cout << "Error: System cannot have negative burst time...\n";
}

int Scheduler::getMemory()
{
	return memoryLimit;
}

int Scheduler::getBurst()
{
	return burstTime;
}

void Scheduler::addProcess(ProgramFile* program)
{
	// Make sure there is enough memory for the process
	if( program->getMemoryRequirements() <= memoryLimit)
	{
		// Make the proc, if there was enough room, add it as normal, otherwise make it in VM
		auto job = std::make_shared<Process>(program, currentTime);

		bool memoryWasFreed = true;

		// Try to get memory for the new job
		if( job->getMemoryRequired() > (memoryLimit - memoryUsage) )
			memoryWasFreed = acquireResources( job->getMemoryRequired() );
		else
			memoryUsage += job->getMemoryRequired();

		// Enough memory was not freed, add the new job to VM
		if( !memoryWasFreed )
			job->shiftToVM();			
		
		// Push the job onto the queue
		runningJobs.push_back( job );
	}
	else
	{
		std::cout << program->getFileName() << " cannot be started, not enough total memory.\n";
	}
}

void Scheduler::finishProcess( const std::shared_ptr<Process>& process)
{
	memoryUsage -= process->getMemoryRequired();
	finishedJobs.push_back(process);
}


void Scheduler::printRunningQueue()
{
	if( runningJobs.empty() )
	{
		std::cout << "The queue is empty\n";
		return;
	}
		

	std::cout << "The queue is:\n";
	int count = 1;

	std::deque<std::shared_ptr<Process>> tempJobs;
	while( !runningJobs.empty() )
	{
		auto j = runningJobs.front();
		runningJobs.pop_front();

		std::cout << "\tPosition " << count++ << ": job " << j->getData() << std::endl;
		tempJobs.push_back(j);
	}
	runningJobs = tempJobs;
}

void Scheduler::printWaitingQueue()
{
	if( waitingOnIOJobs.empty() ) return;

	std::deque<std::shared_ptr<Process>> tempJobs;
	while( !waitingOnIOJobs.empty() )
	{
		auto j = waitingOnIOJobs.front();
		waitingOnIOJobs.pop_front();

		std::cout << "The process " << j->getName() << " is obtaining IO and will be back in " << j->getRemainingIOTime() << " unit.\n";
		tempJobs.push_back(j);
	}
	waitingOnIOJobs = tempJobs;
}

void Scheduler::printFinishedQueue()
{
	if( finishedJobs.empty() ) return;

	std::cout << "Finished Jobs are: \n";

	std::deque<std::shared_ptr<Process>> tempJobs;
	while( !finishedJobs.empty() )
	{
		auto j = finishedJobs.front();
		finishedJobs.pop_front();

		std::cout << "\t" << j->getFinishedData() << std::endl;
		tempJobs.push_back(j);
	}
	finishedJobs = tempJobs;
}


void Scheduler::printSystemUpdate()
{
	std::cout << "\nCurrent time <" << currentTime << ">\n";

	printCurrentJob();		
	printRunningQueue();
	printWaitingQueue();
	printFinishedQueue();
}

bool Scheduler::acquireResources(int amount)
{
	int available = (memoryLimit - memoryUsage);

	if( available >= amount )
	{
		memoryUsage += amount;
		//std::cout << "plenty of memory" << (amount) << "\n";
		return true;
	}
	else if( available + calculateFreeable() >= amount )
	{
		freeMemory( amount - available );
		//std::cout << "Freeing memory " << (amount - available) << "\n";
		memoryUsage += amount;
		return true;
	}

	return false;
}


int Scheduler::calculateFreeable()
{
	int freeable = 0;

	std::deque<std::shared_ptr<Process>> tempJobs;
	while( !runningJobs.empty() )
	{
		auto j = runningJobs.front();
		runningJobs.pop_front();

		if( !j->isInVM())
			freeable += j->getMemoryRequired();

		tempJobs.push_back(j);
	}
	runningJobs = tempJobs;

	return freeable;
}

void Scheduler::freeMemory(int amount)
{
	if( amount < 1 ) return;

	int freed = 0;

	std::deque<std::shared_ptr<Process>> tempJobs;
	while( !runningJobs.empty() )
	{
		auto j = runningJobs.back();
		runningJobs.pop_back();

		if( !j->isInVM() && freed < amount)
			freed += j->shiftToVM();

		tempJobs.push_front(j);
	}
	runningJobs = tempJobs;
	memoryUsage -= freed;
}
