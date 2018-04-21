/*
 *	Andrew McGuiness
 *	ITEC 371 - Project 4
 *	4/19/2018
*/

#include "Scheduler.h"
#include <ostream>
#include <iostream>

/**
 *	\brief Create a new Scheduler with the specified burst and memory.  
 *	
 *	The default burst and memory are invalid to begin with.  The user must set 
 *	them by calling setMemory and setBurst.  This was done as recommended by the 
 *	spec.
 *	
 *	\param mem Amount of memory that the scheduler can allocate to running 
 *	processes
 *	\param burst Amount of time to allocate each process while it's running
 */
Scheduler::Scheduler(int mem, int burst)
	: memoryLimit(mem), burstTime(burst), currentTime(0)
{
}

/**
 *	\brief Run the system until all the jobs on all the queues are finished.
 *	
 *	The system will run in bursts of time.  A process is pulled from the running 
 *	queue and ticked the number of times allowed by the burst.  It is then sent 
 *	to the end of the queue.
 *	
 *	If the job needs IO, it is moved off the running queue and into the IO queue 
 *	where it will run in parallel with the rest of the jobs.
 *	
 *	If the job is in VM, it will be allocated memory before it can run.
 */
void Scheduler::run()
{
	// Ensure that the burst time is at least 1
	if( burstTime <= 0 )
	{
		std::cout << "Burst Time must be greater than 0 before running.\n";
		return;
	}

	std::cout << "\nAdvancing the system until all jobs finished\n";

	// Tick the system until all jobs on all queues done
	while( currentProcess || !runningJobs.empty() || !waitingOnIOJobs.empty())
		tickSystem();

	// Final system update
	printSystemUpdate();
}

/**
 *	\brief Scan the getting IO queue to see if any of the jobs are down with IO.
 *	
 *	If a job on the IO queue has finished getting IO, it will be moved to the 
 *	end of the running queue.  If it is not finished, it will just be kept on 
 *	the IO queue.
 */
void Scheduler::checkForWakingIO()
{
	// Hold the check jobs
	std::deque<std::shared_ptr<Process>> tempJobs;

	// While there are jobs that haven't been checked
	while( !waitingOnIOJobs.empty() )
	{
		// Grab the next job
		auto j = waitingOnIOJobs.front();
		waitingOnIOJobs.pop_front();

		// If it's done with IO, put it on the back of the running queue
		if( j->shouldWakeFromIO() )
			runningJobs.push_back(j);

		// Still has IO to do, put it on the temp
		else
			tempJobs.push_back(j);
	}

	// reset the IO queue
	waitingOnIOJobs = tempJobs;
}

/**
 *	\brief Tick the system clock and run all processes by 1 time unit.
 *	
 *	All jobs on the IO queue will be updated and the running job will
 *	have it's main ticked.  If the current job finishes it will be pushed
 *	to the finished queue, if it needs IO it will be sent to that queue.  If
 *	it has exhausted it's burst, it will be pushed to the back of the running
 *	queue.
 *	
 *	If there is no active job, the next one will be pulled from the running 
 *	queue.  If there are no jobs available, only IO is ticked.
 */
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

		// All jobs are waiting on IO
		else if(!waitingOnIOJobs.empty())
		{
			std::cout << "No processes to run, waiting on IO.\n";
		}

		// Everything has finished
		else
		{
			std::cout << "All queues are empty.\n";
			return;
		}
	}

	// Tick the current job's main by 1 unit
	if( currentProcess )
		currentProcess->tickMain( currentTime, this );

	// Tick all IO jobs by 1 unit
	tickIOJobs();

	// If the current job needs IO, move it to that queue
	if (currentProcess && currentProcess->shouldSleepForIO())
	{
		waitingOnIOJobs.push_back( currentProcess );
		currentProcess = nullptr;
	}

	// If the current job is finished, move it to the finished queue
	if( currentProcess && currentProcess->isFinished())
	{
		finishProcess( currentProcess );
		currentProcess = nullptr;
	}

	// Process all IO jobs to see if any are done
	checkForWakingIO();

	// Update the burst and the system time
	currentTime++;
	remainingBurst--;
}

/**
 *	\brief Tick all the jobs that are waiting on IO.
 *	
 *	IO jobs are checked to see if they should wake in a separate function.  
 *	This function just updates all IO job internal timers.
 */
void Scheduler::tickIOJobs()
{
	// Hold all jobs that have been ticked
	std::deque<std::shared_ptr<Process>> tempJobs;
	while( !waitingOnIOJobs.empty() )
	{
		// Grab the next job
		auto j = waitingOnIOJobs.front();
		waitingOnIOJobs.pop_front();

		// Tick it
		j->tickIO();

		// Put it on the check queue
		tempJobs.push_back(j);
	}
	// Swap the queues
	waitingOnIOJobs = tempJobs;
}

/**
 *	\brief Advance the system forward by a number of time units.
 *	
 *	This is similar to calling the run function.  However, instead of running 
 *	until everything is finished, the system is only ticked a specified number 
 *	of units.
 *	
 *	\amount How many times to tick the system.
 */
void Scheduler::step(int amount)
{
	// Ensure that there is a burst time set
	if( burstTime <= 0 )
	{
		std::cout << "Burst Time must be greater than 0 before running.\n";
		return;
	}

	std::cout << "\nAdvancing the system for " << amount 
		<< " units or until all jobs finished\n";

	// Tick the system until all jobs are done or the amount of ticks is 
	//	exhausted
	while( 	amount > 0 && 
			(	currentProcess 			|| 
				!runningJobs.empty() 	|| 
				!waitingOnIOJobs.empty()
			) )
	{
		tickSystem();
		amount--;
	}

	// Give the user an update on the current system state.
	std::cout << "System state at the end of stepping:\n";
	printSystemUpdate();
}

/**
 *	\brief	Print information on the currently running process. 
 */
void Scheduler::printCurrentJob()
{
	if( currentProcess )
		std::cout << "Running job " << currentProcess->getData() << "\n";
	else
		std::cout << "Running job is empty\n";
}

/**
 *	\brief Set the amount of memory the scheduler can allocate to jobs.
 *	
 *	\param amount How much memory to have
 */
void Scheduler::setMemory(int amount)
{
	// Ensure that the number is not negative
	if( amount < 0)
		std::cout << "Error: System cannot have negative memory.\n";

	// Ensure there are no jobs currently using memory
	if( memoryUsage > 0)
	{
		std::cout << "Error: Job are currently using memory.  Please allow them" 
			<< " to finish before changing memory limit.\n";
	}

	// Everything is good, set the memory
	else
		memoryLimit = amount;
}

/**
 *	\brief Set the amount of time each process is allocated to run in.
 *	
 *	\param amount How much time to give a job when it is running.
 *	
 *	The system must have a burst time of at least 2.  This is because a job 
 *	requires 2 ticks to be loaded from VM.  If the burst time is less than 2, 
 *	jobs will never get loaded.
 */
void Scheduler::setBurst(int amount)
{
	// Ensure that the burst is above the minimum
	if( amount >= 2)
	{
		// Set burst and reset remaining
		burstTime = amount;
		remainingBurst = burstTime;
	}

	// Bad burst time
	else
		std::cout << "Error: System must have a burst time of at least 2.\n";
}

/**
 *	\return The amount of memory that the scheduler has to allocate to jobs.
 */
int Scheduler::getMemory()
{
	return memoryLimit;
}

/**
 *	\return The amount of time the scheduler will allocate to a job before it 
 *	is swapped.
 */
int Scheduler::getBurst()
{
	return burstTime;
}

/**
 *	\brief Add a ProgramFile to the scheduler's job queue.
 *	
 *	\param program A ProgramFile that should be converted into a process and 
 *	added to the scheduler.
 *	
 *	The ProgramFile will be checked to ensure that the system has enough memory 
 *	to run the Program.  If it does, the Program will be converted into a 
 *	process.  This process will be allocated memory.  If there is not enough 
 *	available, the scheduler will use the RMU algorithm to shift a process 
 *	into VM.
 */
void Scheduler::addProcess(ProgramFile* program)
{
	// Make sure there is enough memory for the process to run at all
	if( program->getMemoryRequirements() <= memoryLimit)
	{
		// Make the proc, if there was enough room, add it as normal, otherwise 
		//	make it in VM
		auto job = std::make_shared<Process>(program, currentTime);

		// Flag, is there enough free memory on the system after we moving 
		//	stuff to VM
		bool enoughMemory = true;

		// There is not enough free memory, move stuff to VM
		if( job->getMemoryRequired() > (memoryLimit - memoryUsage) )
			enoughMemory = acquireResources( job->getMemoryRequired() );
		// There is plenty of free memory, just add the job
		else
			memoryUsage += job->getMemoryRequired();

		// Enough memory was not freed, add the new job to VM right off the bat
		if( !enoughMemory )
			job->shiftToVM();			
		
		// Push the job onto the queue
		runningJobs.push_back( job );
	}

	// The system doesn't have enough memory to run the program
	else
	{
		std::cout << program->getFileName() 
			<< " cannot be started, not enough total memory.\n";
	}
}

/**
 *	\brief A process has finished working, move it to the finished queue and 
 *	free it's memory.
 *	
 *	\param process The process that has finished it's job.
 */
void Scheduler::finishProcess( const std::shared_ptr<Process>& process)
{
	// Free the job's memory and push it onto the finished queue
	memoryUsage -= process->getMemoryRequired();
	finishedJobs.push_back(process);
}

/**
 *	\brief Print data for each process on the Running job's queue.
 *	
 *	The output from this function is based on the Project Spec.
 */
void Scheduler::printRunningQueue()
{
	// There are no jobs, handle special case
	if( runningJobs.empty() )
	{
		std::cout << "The queue is empty\n";
		return;
	}
		
	// Output based on Project Spec
	std::cout << "The queue is:\n";
	int count = 1;

	// Process each job on the running queue
	std::deque<std::shared_ptr<Process>> tempJobs;
	while( !runningJobs.empty() )
	{
		// Pop job off
		auto j = runningJobs.front();
		runningJobs.pop_front();

		// Print it's data
		std::cout << "\tPosition " << count++ << ": job " 
			<< j->getData() << std::endl;

		// Push it onto the processed queue
		tempJobs.push_back(j);
	}

	// Swap queues
	runningJobs = tempJobs;
}

/**
 *	\brief Print data for each process on the IO queue.
 *	
 *	The output from this function is based on the Project Spec.
 */
void Scheduler::printWaitingQueue()
{
	// There are no jobs doing IO, cool.
	if( waitingOnIOJobs.empty() ) return;

	// Store the processed jobs
	std::deque<std::shared_ptr<Process>> tempJobs;
	while( !waitingOnIOJobs.empty() )
	{
		// Pop of the next job
		auto j = waitingOnIOJobs.front();
		waitingOnIOJobs.pop_front();

		// Print it's data
		std::cout << "The process " << j->getName() 
			<< " is obtaining IO and will be back in " 
			<< j->getRemainingIOTime() << " unit.\n";
		
		// Push it onto the processed queue
		tempJobs.push_back(j);
	}

	// Swap the queues
	waitingOnIOJobs = tempJobs;
}

/**
 *	\brief Print data for each process on the finished queue.
 *	
 *	The output from this function is based on the Project Spec.
 */
void Scheduler::printFinishedQueue()
{
	// No jobs have been finished yet.
	if( finishedJobs.empty() ) return;

	std::cout << "Finished Jobs are: \n";

	// Queue to hold the processed jobs
	std::deque<std::shared_ptr<Process>> tempJobs;
	while( !finishedJobs.empty() )
	{
		// Pop of the next job
		auto j = finishedJobs.front();
		finishedJobs.pop_front();

		// Print it's data
		std::cout << "\t" << j->getFinishedData() << std::endl;

		// Push it onto the processed queue
		tempJobs.push_back(j);
	}
	// Swap the queues
	finishedJobs = tempJobs;
}

/**
 *	\brief Print spec formatted output for a "System Update".
 */
void Scheduler::printSystemUpdate()
{
	// Print how many times the system has been ticked since it started
	std::cout << "\nCurrent time <" << currentTime << ">\n";

	// Print data on the current job and all queues
	printCurrentJob();		
	printRunningQueue();
	printWaitingQueue();
	printFinishedQueue();
}

/**
 *	\brief Attempt to acquire resources for a process.  This is done by shifting
 *	jobs into VM which frees their memory.
 *	
 *	\param amount How much memory needs to be acquired
 *	
 *	There is a chance that the required memory cannot be acquired.  If there 
 *	are jobs getting IO, they are not considered for a VM shift.  Likewise, a 
 *	job that is currently running cannot be moved to VM.
 *	
 *	\return True if the memory was acquired (freed from other processes)
 */
bool Scheduler::acquireResources(int amount)
{
	// Calculate how much memory we DO have
	int available = (memoryLimit - memoryUsage);

	// There is plenty of memory, no need to free anything
	if( available >= amount )
	{
		// add the memory usage
		memoryUsage += amount;
		return true;
	}

	// Calculate how much can be freed and see if that's enough
	if( available + calculateFreeable() >= amount )
	{
		// It's enough, so free the minimum we can using MRU algorithm
		freeMemory( amount - available );
		memoryUsage += amount;
		return true;
	}

	return false;
}

/**
 *	\brief Calculate how much memory can be taken from jobs that are on the 
 *	running queue.
 *	
 *	Only jobs that are currently on the running queue but that are not actually 
 *	running are considered for freeing.  A job that is already in VM is also 
 *	ignored.
 *	
 *	\return How much memory can be freed from the system maximum.
 */
int Scheduler::calculateFreeable()
{
	int freeable = 0;

	// Queue to hold processed jobs
	std::deque<std::shared_ptr<Process>> tempJobs;
	while( !runningJobs.empty() )
	{
		// Step through each job on the running queue
		auto j = runningJobs.front();
		runningJobs.pop_front();

		// If it's not in VM it can be freed, add it to the total
		if( !j->isInVM())
			freeable += j->getMemoryRequired();

		// Push the job onto the processed queue
		tempJobs.push_back(j);
	}
	//Swap the queues
	runningJobs = tempJobs;

	// Return the max amount of memory that can be freed
	return freeable;
}

/**
 *	\brief Start shifting jobs into VM until we have the minimum amount of
 *	memory required.
 *	
 *	This uses the MRU algorithm to shift jobs into the VM.  This is not the most 
 *	efficient way of doing it, as it might free more memory than needed.
 *	
 *	\param amount How much memory was actually freed.  This may be more than 
 *	was requested.
 */
void Scheduler::freeMemory(int amount)
{
	// Freeing 0 memory is super easy... Done.
	if( amount < 1 ) return;

	int freed = 0;

	// Queue to hold processed jobs
	std::deque<std::shared_ptr<Process>> tempJobs;
	while( !runningJobs.empty() )
	{
		// Step through all jobs on the running queue
		auto j = runningJobs.back();
		runningJobs.pop_back();

		// If the job can be freed and we still need more memory, free it
		if( !j->isInVM() && freed < amount)
			freed += j->shiftToVM();

		// Add the job to the processed queue
		tempJobs.push_front(j);
	}
	// Swap the queues
	runningJobs = tempJobs;

	// Adjust the system's usage
	memoryUsage -= freed;
}
