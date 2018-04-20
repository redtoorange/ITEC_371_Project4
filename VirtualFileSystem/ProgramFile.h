/*
 *	Andrew McGuiness
 *	ITEC 371 - Project 4
 *	4/19/2018
*/

#ifndef PROGRAM_FILE_H
#define PROGRAM_FILE_H

#include <memory>
#include "File.h"

/**
*	\brief A special type of file that contains the necessary metadata to be executed.  ProgramFiles can be loaded
*	into a Scheduler which will spawn a process, an object that contains running process data.
*/
class ProgramFile : public File
{
public:
	/**
	 *	\brief Create a new ProgramFile with the given parameters.  The program file will
	 *	be created on the heap and a shared point will be returned.
	 * 
	 *	\param name Name of the file
	 *	\param time How long does the program take to run
	 *	\param mem	How much memory does the program require to run
	 *	\param doesIO 1 if the program does IO, 0 otherwise
	 *	\param timeIO if the program does IO, when should it start?
	 *	\param amountIO  if the program does IO, how long does it take?
	 *	
	 *	\return A shared pointer to a heap allocated ProgramFile.
	 */
	static std::shared_ptr<ProgramFile> makeProgramFile( const std::string& name, int time, int mem, int doesIO, int timeIO, int amountIO );
	
	/**
	 *	\brief Inflate a stored ProgramFile back into a full file.
	 *	
	 *	\param name The name of the ProgramFile that is read from the file.
	 *	\param stream The stream that contains the rest of the data.
	 *	
	 *	The ProgramFile is read from a binary file and reconstructed from the data.
	 *	
	 *	\return A shared pointer to a ProgramFile to be managed by a Directory
	 */
	static std::shared_ptr<ProgramFile> inflateProgramFile( std::string& name, std::ifstream& stream );

	/**
	 *	\brief Validate a user provided name to ensure that it complies with spec.
	 * 
	 *	\param name Program name entered by the user.
	 *	
	 *	The system would attempt to create a new program with the given name.  
	 *	The system will accept just a name or one with an extension.  The name will
	 *	be validated and trimmed.  
	 *	
	 *	\return True if the ProgramFile name is valid.
	 */
	static bool validName( std::string* name);

	/**
	 *	\brief Should NOT be used.  Instead use the makeProgramFile function.
	 * 
	 *	This should not be used, as it does not fully validate the file name and the
	 *	ProgramFile will be stack allocated which will make it difficult to serialize later.
	 * 
	 *	\param name Name of the ProgramFile
	 *	\param timeReq How long does this Program take to run
	 *	\param memReq How much memory does it need to run
	 *	\param doesIO Does it need to do IO?
	 *	\param timeIO When does it fo IO?
	 *	\param amountIO How long for IO?
	 */
	ProgramFile(const std::string& name, int timeReq, int memReq, int doesIO, int timeIO, int amountIO);

	/**
	 *	\brief Print the file's data based on the what is required from the spec.
	 *	
	 *	\param tabs How many tabs to include when printing.  Helps with displating things in a heirarchy
	 */
	void printData(int tabs) override;
	
	/**
	 *	\brief Write the program file out to a stream so that is can be saved to disk.
	 *	
	 *	\param stream The output file stream to write the program's data to.
	 */
	void writeToFile(std::ofstream& stream) override;

	/**
	 *	\brief Get how much memory this program requires on the scheduler to run.
	 *	
	 *	\return Amount of memory required for this program to tick
	 */
	int getMemoryRequirements() const;

	/**
	 *	\brief Get the number of time units this program needs to run for to finish.
	 *	
	 *	This does NOT include any IO requirements.
	 *	
	 *	\return How long does this program required to be running to finish.
	 */
	int getTimeRequirements() const;

	/**
	 *	\brief Does this program do any form of IO?
	 *	
	 *	Encoded true and false into 1 and 0 to simplify serialization.
	 *	
	 *	\return 0 = this program doesn't need IO, 1 = this program needs IO.
	 */
	int getNeedsIO() const;
	
	/**
	 *	\brief What time does this program need to go do IO.
	 *	
	 *	\return At what tick does this Program need IO
	 */
	int getTimeToDoIO() const;
	
	/**
	 *	\brief How long will this program need to perform IO once it's started.
	 *	
	 *	\return How long should the Program perform IO.
	 */
	int getAmoutOfIO() const;

private:
	//! How much memory does this program require to execute
	int memoryRequirements;
	//! How long does it take for this process to execute
	int timeRequirements;

	//! Does this process need to fetch IO
	int needsIO;
	//! When does this process need to start fetching IO
	int timeToDoIO;
	//! How long will this process be fetching IO for
	int amoutOfIoTime;
};

#endif