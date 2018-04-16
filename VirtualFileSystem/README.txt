CONTENTS OF THIS FILE
---------------------
 1) Introduction
 2) Requirements
 3) Setup
 4) File Description
 
1) Introduction
---------------------

The files in this directory are part of project 3 for ITEC 371 at Radford 
	University.  The goal of the project was to create a simulated Shell 
	environment, in the vein of a Linux Bash shell.

The Program attempts to load in a binary file specified by the user from 
	the local directory.  This file has a very specific format that must be 
	followed to work.  The exact specifications were supplied to use as part 
	of Project 1.  If the specified file is not present, a new one will be 
	created so it can be written to.

The file system resides entirely in memory once it has been loaded from the 
	Binary File.  The system remains there until the user quits the shell.  At 
	the point, the entire tree is compressed back down into a binary file and 
	saved to disk.

The shell will allow a variety of different instructions:
	pwd – Print the absolute path of the current directory from the root 
		directory
	
	ls – List the names of all of the files and directories stored in the 
		current directory, include the type (file, directory, etc). Also list 
		the name of the directory.
	
	cd <dirname> - Change the current working directory to the 
		specified directory
	
	cd .. – Change the current working directory to the previous working 
		directory (or ignore if in the root director)
	
	mkdir <name> - Create a directory in the current working directory
	
	cat <filename> - Print the contents of the text file to the screen or 
		print out invalid file type
	
	createTextfile <filename> - Create a text file of the particular name, 
		and prompt the user for its contents
	
	start <program> - Add a program to the scheduler to manage and run.
	
	step <number> - Advanced the scheduler and the system be thespecified
		number of time units.
		
	setBurst <number> - Set the amount of time a process will be allocated by
		the scheduler before it is swapped by the next process.
		
	setMemory <number> - Set the total amount of memory that the system has
		to allocate to all processes.
		
	getMemory	 - Get the total amount of memory that the system has.
	
	getBurst - Get the amount of time processes are allocated before they are
		swapped.
		
	run - Run the simulation until all jobs are completed.
	
	addProgram <name> <timeRequirement> <memoryRequirement> - Add a program to
		the current directory.  It will require the amount of time and memory to run.
		Optional Parameter: <timeToDoIO> <AmountOfIOTime> - must both be given as
			integers.  This specifies when the process will sleep for IO and for how long.
	
2) Requirements
---------------------

A version of g++ compatible with c++11 is required to compile the system.  
	The system makes extensive use of modern c++11 constructs, so it cannot 
	be compiled without them.


3) Setup
---------------------

To build the system, just use the make command from a terminal.  The make 
	file will instruct the compiler/linker on how to build the system.

Once the program has been built, it can be executed from a terminal with 
	./RATCH <filename>
	
	* Replace <filename> with the name of the file you want to load/save to.  
	The extension does not matter.

4) File Description
---------------------
	main.cpp
		The main entry point for the program.  main.cpp contains the code that
		is used to load a file system object from the disk.  It also contains 
		the main loop of execution.
		
	Util.*
		Contains simple helper functions and global constant data.  Util.h 
		contains a a description of all possible instructions that the shell will 
		accept.
		
	FSObject.*
		Described a generic FileSystemObject.  Directories and Files 
		inherit from the FSObject.  All objects in the system are 
		FSObjects.  This allows the use of generic containers to hold 
		all types of FSObjects.
		
	Directory.*
		Extends FSObject, represents a folder in the directory.
		
	File.*
		Extends FSObjects, represents any non-directory object on the drive.
		
	TextFile.*
		A file with specific operations for handling the "metadata."
	
	ProgramFile.*
		Contains the data about a program.  How long it needs to run, how much
		memory it requires and how to handle IO.
		
	Process.*
		When a program is executed, a process object is spawned to manage the
		instance of the program.  The process tracks when it was executed, for 
		how long and it's current status.
		
	Scheduler.*
		The system has a single scheduler that manages all running processes
		on the system.  The scheduler has queues to track running, waiting
		and finished jobs on the system.
