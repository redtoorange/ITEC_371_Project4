/*
 *	Andrew McGuiness
 *	ITEC 371 - Project 4
 *	4/19/2018
*/

#include "ProgramFile.h"
#include <iostream>
#include <string>

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
std::shared_ptr<ProgramFile> ProgramFile::makeProgramFile( const std::string& name, int time, int mem, int doesIO, int timeIO, int amountIO  )
{
	// Check the name of the file
	std::string n = name;
	if( !validName(&n) )
	{
		std::cout << "Invalid program name!" << std::endl;
		return nullptr;
	}

	// Send back out new ProgramFile pointer
	return std::make_shared<ProgramFile>(n, time, mem, doesIO, timeIO, amountIO);

}

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
 *	
 */
ProgramFile::ProgramFile( const std::string& name, int timeReq, int memReq, int doesIO, int timeIO, int amountIO)
{
	fileName = name;

	timeRequirements = timeReq;
	memoryRequirements = memReq;

	needsIO = doesIO;
	timeToDoIO = timeIO;
	amoutOfIoTime = amountIO;
}

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
bool ProgramFile::validName( std::string* name)
{
	// Strip the extension
	auto back = name->end();
	if( *--back == 'p' && *--back == '.')
	{
		name->erase(back, name->end());
	}

	// Check the length
	if( name->length() > 8)
	{
		std::cout << "File names cannot exceed 8 characters.\n";
		return false;
	}
		
	// Verify all the characters are valid
	auto iter = name->begin();
	for(;iter != name->end(); ++iter)
	{
		// Step through each char and validate it
		char c = *iter;
		if( !((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||((c >= '0' && c <= '9')) ) )
		{
			// Found an error, notify the user
			std::cout << "File names must be alpha-numeric.\n";
			return false;
		}
	}

	return true;
}

/**
 *	\brief Print the file's data based on the what is required from the spec.
 *	
 *	\param tabs How many tabs to include when printing.  Helps with displating things in a heirarchy
 */
void ProgramFile::printData(int tabs)
{
	// Make us some sweet sweet tabs
	std::string t;
	for( int i = 0; i < tabs; i++)
		t += "\t";

	// Print the normal program data
	std::cout << t << fileName << ".p" << std::endl;
	std::cout << t << "\t" << "Time Requirement: " << timeRequirements << std::endl;
	std::cout << t << "\t" << "Mem Requirement : " << memoryRequirements << std::endl;

	// If the Program does IO, print it's IO data
	if( needsIO)
	{
		std::cout << t << "\t" << "IO Time   : " << timeToDoIO << std::endl;
		std::cout << t << "\t" << "IO Amount : " << amoutOfIoTime << std::endl;
	}
}

/**
 *	\brief Write the program file out to a stream so that is can be saved to disk.
 *	
 *	\param stream The output file stream to write the program's data to.
 */
void ProgramFile::writeToFile(std::ofstream& stream)
{
	// Write the name to the stream
	stream << fileName;

	// fill the remaining space with null chars
	for( int i = fileName.length(); i < 8; i++)
	{
		stream << '\0';
	}

	// Add the spec required extension and null character
	stream << ".p";
	stream << '\0';

	// Write the objects state into the file
	stream.write((char*)&timeRequirements, sizeof(timeRequirements));
	stream.write((char*)&memoryRequirements, sizeof(memoryRequirements));

	// Write the IO data out
	stream.write((char*)&needsIO, sizeof(needsIO));
	stream.write((char*)&timeToDoIO, sizeof(timeToDoIO));
	stream.write((char*)&amoutOfIoTime, sizeof(amoutOfIoTime));
}

/**
 *	\brief Get how much memory this program requires on the scheduler to run.
 *	
 *	\return Amount of memory required for this program to tick
 */
int ProgramFile::getMemoryRequirements() const
{
	return memoryRequirements;
}

/**
 *	\brief Get the number of time units this program needs to run for to finish.
 *	
 *	This does NOT include any IO requirements.
 *	
 *	\return How long does this program required to be running to finish.
 */
int ProgramFile::getTimeRequirements() const
{
	return timeRequirements;
}

/**
 *	\brief Does this program do any form of IO?
 *	
 *	Encoded true and false into 1 and 0 to simplify serialization.
 *	
 *	\return 0 = this program doesn't need IO, 1 = this program needs IO.
 */
int ProgramFile::getNeedsIO() const
{
	return needsIO;
}

/**
 *	\brief What time does this program need to go do IO.
 *	
 *	\return At what tick does this Program need IO
 */
int ProgramFile::getTimeToDoIO() const
{
	return timeToDoIO;
}

/**
 *	\brief How long will this program need to perform IO once it's started.
 *	
 *	\return How long should the Program perform IO.
 */
int ProgramFile::getAmoutOfIO() const
{
	return amoutOfIoTime;
}

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
std::shared_ptr<ProgramFile> ProgramFile::inflateProgramFile(std::string& name, std::ifstream& stream)
{
	std::string n = name;
	validName(&n);

	// Load process data
	int t;
	int m;

	// Load IO Data
	int doesIO;
	int timeIO;
	int amountIO;
	
	// Load the object's state from the file
	stream.read((char*)&t, sizeof(t));
	stream.read((char*)&m, sizeof(m));

	// Load in the IO data
	stream.read((char*)&doesIO, sizeof(doesIO));
	stream.read((char*)&timeIO, sizeof(timeIO));
	stream.read((char*)&amountIO, sizeof(amountIO));

	// Return the shared pointer to a new ProgramFile
	return std::make_shared<ProgramFile>(n, t, m, doesIO, timeIO, amountIO);
}