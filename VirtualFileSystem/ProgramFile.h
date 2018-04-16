/*
 *	Andrew McGuiness
 *	ITEC 371 - Project 3
 *	4/1/2018
*/

#ifndef PROGRAM_FILE_H
#define PROGRAM_FILE_H

#include <memory>

#include "File.h"

/*!
*	\brief A special type of file that contains the necessary metadata to be executed.  ProgramFiles can be loaded
*	into a Scheduler which will spawn a process, an object that contains running process data.
*/
class ProgramFile : public File
{
public:
	//! Create a new ProgramFile given a filename, user will be prompted for input
	static std::shared_ptr<ProgramFile> makeProgramFile( const std::string& name, int time, int mem, int doesIO, int timeIO, int amountIO );
	
	//! Inflate a stored ProgramFile back into a full file
	static std::shared_ptr<ProgramFile> inflateProgramFile( std::string& name, std::ifstream& stream );

	//! Ensure that the supplied name is valid
	static bool validName( std::string* name);

	//! Should NOT be used outside of the class
	ProgramFile(const std::string& name, int timeReq, int memReq, int doesIO, int timeIO, int amountIO);

	//! Formatted data printer
	void printData(int tabs) override;
	
	//! Write the program file out to a stream
	void writeToFile(std::ofstream& stream) override;

	//! Get how much memory this program requires on the scheduler to run
	int getMemoryRequirements() const;

	//! Get the number of time units this program needs to run for to finish
	int getTimeRequirements() const;

	//! 0 = this program doesn't need IO, 1 = this program needs IO
	int getNeedsIO() const;
	
	//! What time does this program need to go do IO
	int getTimeToDoIO() const;
	
	//! How long will this program need to perform IO
	int getAmoutOfIO() const;

private:
	int memoryRequirements;
	int timeRequirements;

	int needsIO;
	int timeToDoIO;
	int amoutOfIoTime;
};

#endif