/*
 *	Andrew McGuiness
 *	ITEC 371 - Project 4
 *	4/19/2018
*/

#include <istream>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Util.h"
#include "Directory.h"
#include "TextFile.h"
#include "Scheduler.h"
#include "ProgramFile.h"

using  std::cout; using  std::cin; using  std::endl; using  std::string;

//! Should the Application be running?
bool running = true;

//! Holds a pointer to the current directory
Directory* currentDirectory;

//! Holds a pointer to the root for comparison during runtime
Directory* rootPointer;

//! The system's Scheduler manages all running, waiting and finished processes
Scheduler scheduler;


/**
 *	\brief Attempt to create a new directory inside of currentDirectory.
 *	
 *	If the directory is made successfully, then it will be placed inside 
 *	the current directory, then the currentDirectory pointer will be set 
 *	to point at the new directory.
 *	
 *	\param dirName Name that the Directory should have.
 */
void createDir( const std::string& dirName)
{
	// Attempt to create a new Directory with the given name
	const auto dir = Directory::CreateDirectory(dirName, currentDirectory );

	// Swap currentDirectory only if d was created
	if( dir )
		currentDirectory->addObject( dir );
}

/**
 *	\brief Attempt to create a new file inside currentDirectory.  There needs to be a 
 *	touch of parsing done here to direct control flow. 
 */
void createTextFile( )
{
	cout << "Enter filename>";
		
	string fileName;
	getline(cin, fileName);

	// Make sure that the user entered something
	if( fileName.empty() )
	{
		cout << "Filename cannot be empty." << endl;
		return;
	}


    // Parse the last two chars of the filename
	int len = fileName.length();
	if( fileName.substr(len-2) == ".t")
	{
		// Make the actual TextFile and add it to the currentDirectory
		const auto file = TextFile::makeTextFile(fileName);
		if( file )
			currentDirectory->addObject( file );
	}

	// Conform to spec, require .t extensions on filenames
	else
	{
		cout << "TextFiles must end in .t" << endl;
	} 
}

/**
 *	\brief Parse user input into a programFile, then store the ProgramFile in the current directory.
 *	
 *	\param programData The string of user input to pull data from.
 */
void createProgramFile( const std::string& programData )
{
	// Tokenize the user's input string
	std::stringstream ss{programData};
	string item;
	std::queue<string> tokens;

	// Parse tokens out of the program line
	while( std::getline(ss, item, ' '))
		tokens.push(item);

	// Ensure we have atleast 3 tokens: Name, time and memory
	if( tokens.size() < 3)
	{
		cout << "Error: Program files must be in format [name] [time] [memory]\n";
	}
	else
	{
		// Parse the three required fields
		string name{tokens.front()};
		tokens.pop();

		int time = std::stoi(tokens.front());
		tokens.pop();

		int mem = std::stoi(tokens.front());
		tokens.pop();

		// Attempt to parse out the IO paramaters if they exist
		int doesIO = 0;
		int timeIO = 0;
		int amountIO = 0;

		if( tokens.size() >= 2)
		{
			doesIO = 1;
			timeIO = std::stoi(tokens.front());
			tokens.pop();
			amountIO = std::stoi(tokens.front());
			tokens.pop();
		}

		// Create the ProgramFile and store it in the current directory
		const auto file = ProgramFile::makeProgramFile(name, time, mem, doesIO, timeIO, amountIO);
		if( file )
			currentDirectory->addObject( file );
	}
	
}

/**
 *	\brief Attempt to change to the directory to the requested directory name.
 *	
 *	If the user is currently inside of the root directory and they try to change
 *	directory to parent (with "..") the command is just ignored.
 *	
 *	\param dirName Name of the directory to change to.
 */
void tryToChangeDir( const std::string& dirName )
{
	// Special case if in root and trying to go up a level
	if(dirName == ".." && rootPointer == currentDirectory)
		return;

	// Search for the requested directory
	Directory* dir = currentDirectory->getDirectory(dirName);

	// Found the directory, change to it
	if( dir )
		currentDirectory = dir;

	// The dir wasn't found, let the user know
	else
		cout << "Could not change Directory to <" << dirName << ">" << std::endl;
}

/**
 *	\brief Search for a TextFile in the current Directory then print it's contents if it was found.
 *	
 *	\param fileName TextFile name to search for and print the contents of.
 */
void printTextFile(const string& fileName)
{
	// Try to get the file
	TextFile* text = currentDirectory->getTextfile( fileName );

	// Print the file if it exists
	if( text )
		cout << "Text file contents:\n" << text->getContents() << endl;
	else
		cout << "Could not read from <" << fileName << ">" << endl;
}

/**
 *	\brief Search for a ProgramFile in the current Directory, if found try to create a process in the Scheduler.
 *	
 *	\param fileName ProgramFile name to search for and spawn a process from.
 */
void startProcess(const string& fileName)
{
	// Try to get the program file
	ProgramFile* program = currentDirectory->getProgramfile( fileName );

	// Send the program file to the scheduler to try and create a process
	if( program )
		scheduler.addProcess( program );	
	else
		cout << "Could not find <" << fileName << "> \n";
}

/**
 *	\brief Handle complex, multipart commands that take an argument.
 *	
 *	\param command What the command was parsed as.
 *	\param input What the user actually typed in.
 *	
 *	\return True of the command was handled successfully.
 */
bool handleCompound(Commands command, const std::string& input )
{
	// Get the length of the command + 1 for a space
	const auto len = cmd[command].length()+1;

	// Ensure that the user didn't JUST enter the command, there needs to be more
	bool valid = input.length() > len && input[len-1] == ' ';

	// Was the command handled?
	bool handled = true;

	if( valid )
	{
		switch( command )
		{
			// Make a new Directory
			case MKDIR:
				createDir( input.substr(len));
				break;

			// Print the contents of a TextFile
			case CAT:
				printTextFile(input.substr(len));
				break;

			// Add a new program to the currentDirectory
			case ADD_PRO:
				createProgramFile(input.substr(len));
				break;

			// Fire up the scheduler and let it run
			case START:
				startProcess(input.substr(len));
				break;

			// Step the scheduler a few ticks
			case STEP:
				scheduler.step( std::stoi(input.substr(len)));
				break;

			// (Attempt to) Change the currentDirectory
			case CD:
				tryToChangeDir( input.substr(3) );
				break;

			// Set the system memory limit
			case SET_MEM:
				scheduler.setMemory( std::stoi(input.substr(len)));
				break;

			// Set the system's burst time
			case SET_BURST:
				scheduler.setBurst( std::stoi(input.substr(len)));
				break;

			default:
				handled = false;
		}
	}

	// Unable to handle the command in it's state
	else
		cout << "Error: Malformed input, <command filename/directory> is required format.\n";

	return handled;
}

/**
 *	\brief Handle simple commands that do not take any arguments.
 *	
 *	\param command What the command was parsed as.
 *	\param input What the user actually typed in.
 *	
 *	\return True of the command was handled successfully.
 */
bool handleSimple(Commands command, const std::string& input )
{
	// Was the command handled?
	bool handled = true;

	switch( command )
	{
		// Create a new textfile, the user will be prompted for furth input
		case CREATE_TEXT:
			if( equalIC(input, "createtextfile"))
				createTextFile();
			else
				cout << "Error: Malformed input, createTextFile takes no arguments." << std::endl;
			break;

		// List the file in currentDirectory
		case LIST:
			if( equalIC(input, "ls"))
				currentDirectory->printData(0);
			else
				cout << "Error: Malformed input, <ls> is required format." << std::endl;
			break;

		// Display the current working directory to the user
		case PWD:
			if( equalIC(input, "pwd"))
				cout << "Current directory is " << *currentDirectory << endl; 
			else
				cout << "Error: Malformed input, <pwd> is required format." << std::endl;	
			break;

		// Run the current stuff in the scheduler
		case RUN:
			scheduler.run();
			break;

		// Display the system's memory
		case GET_MEM:
			{
				int m = scheduler.getMemory();
				if( m == -1 )
					cout << "System Memory has not been configured." << endl;
				else
					cout << "System Memory: " << m << endl;
			}
			break;

		// Display the scheduler's burst time
		case GET_BURST:
			{
				int b = scheduler.getBurst();
				if( b == -1 )
					cout << "System Burst has not been configured." << endl;
				else
					cout << "System Burst Time: " << b << endl;
			}
			break;

		// Quit the program and write the current FS to disk
		case QUIT:
			running = false;
			break;

		default:
			handled = false;
	}

	return handled;
}

/**
 *	\brief Direct to control flow based on the category of the command that the user entered.
 *	
 *	\param command What the command was parsed as.
 *	\param input What the user actually typed in.
 *	
 *	Commands can either be complex or simple.  Simple commands only contain the command, like "ls" or "pwd".
 *	These commands can be executed directly.  Complex commands take arguments and need to be further
 *	parsed.
 *	
 *	\return True of the command was handled successfully.
 */
bool executeCommand( Commands command, const std::string& input )
{
	switch(command)
	{
		// Handle all complex commands
		case MKDIR: case CAT:  case START:  case CD: case ADD_PRO: case SET_MEM: case SET_BURST: case STEP:
				return handleCompound(command, input);

		// Handle all simple commands
		case CREATE_TEXT: case LIST: case PWD: case RUN: case GET_MEM: case GET_BURST:  case QUIT:
				return handleSimple(command, input);
	}

	return false;
}

/**
 *	\brief The program should look for and inflate the FS that is stored on disk. This processes rebuilds 
 *	the FS structure based on the requirements.
 *	
 *	\param filename The name that was passed in by the user as a commandline argument.
 *	
 *	\return The root directory of the reconstructed FS.
 */
std::shared_ptr<Directory>  readInFile( const string& filename )
{
	std::shared_ptr<Directory> root = nullptr;

	// Load in the file from disk
	std::ifstream inFile{ filename, std::ios::in | std::ios::binary};

    // 11 char token to pull from the file.  All files/directories are identified by 11 chars, 8 for name,
    //  a period, a 1 char extension and then a null character
	char token[11];

    // Once we will out the token, we have to toss out the null chars to parse it easier
	std::string parsed;
	
	while( running == true)
	{
        // Grab a token
		inFile.read( token, sizeof(char)*11 );
		parsed.clear();

		// Strip the null chars
		for( int i = 0; i < 11; i++)
		{
			if( token[i] != '\0')
				parsed += token[i];
		}

        // Prep an iterator
		auto extension = parsed.substr(parsed.size()-2);

        // We found a directory
		if( extension == ".d" )
		{
            // Special case for root.d
			if( !currentDirectory )
			{
				root = Directory::CreateDirectory( parsed, nullptr, true);
				currentDirectory = root.get();
				rootPointer = root.get();
			}
			else
			{
                // Try to inflate the flat directory back into the linked structure
				auto d = Directory::CreateDirectory(parsed, currentDirectory );

				// Swap currentDirectory only if d was created
				if( d )
				{
					currentDirectory->addObject( d );
					currentDirectory = d.get();
				}
            }

            // We get the number of files under the directory, but it's not used
			int data;
			inFile.read((char*)&data, sizeof(data));
		}

        // We found a textfile
		else if( extension == ".t" )
			currentDirectory->addObject( TextFile::inflateTextFile(parsed, inFile));

        // REMOVED: We found a program
		else if( extension == ".p")
			currentDirectory->addObject( ProgramFile::inflateProgramFile(parsed, inFile));
	
        // found an endX token, close the current directory
        else if( parsed.find_first_of("end" + currentDirectory->getFileName()) == 0)
		{
			if( currentDirectory == rootPointer )
				running = false;
			else
				currentDirectory = currentDirectory->getParent();
		}
	}

	inFile.close();

	return root;
}

/**
 *	\brief Check the input string for any extra whitespaces at the beginning or end, and remove them.
 *	
 *	\param input The string to remove white spaces from
 */
void trimWhiteSpace(std::string& input)
{
	// Remove leading whitespace
	while( !input.empty() && input[0] == ' ')
		input.erase(0, 1);

	// Remove trailing whitespace
	while( !input.empty() && input[ input.length() - 1 ] == ' ')
		input.erase( input.length() - 1);
}

/**
 *	\brief Enter into the main command loop for the application.  The progra, will continue to listen 
 *	for input until the "quit" command is entered.
 *	
 *	\param filename The name of the filesystem when it is saved.
 *	\param rootPtr Pointer to the root directory of the loaded FS.
 *	
 *	The program can begin execution with a filesystem loaded from disk or with none loaded.  If there
 *	is a FS on disk, it is reinflated and used as root.  If there isn't one already, a new one is created
 *	and will be saved to the supplied filename.
 */
void commandLoop( const string& filename, std::shared_ptr<Directory> rootPtr )
{
	// Start the running flag
	running = true;

	// Set root to what was passed in 
	std::shared_ptr<Directory> root = rootPtr;

	// If null was passed, there is no existing FS, so make a new one
	if( !root )
		root = Directory::CreateDirectory("root", nullptr, true);

	// Initialize the Directory pointers
	currentDirectory = root.get();
	rootPointer = root.get();
	
    // Loop until Quit was entered
    while( running )
	{
		// Read in command
		cout << "EnterCommand>";
		std::string input;
		getline(cin, input);

		bool handled = false;
		
		// Ensure there is no extra whitespace
		trimWhiteSpace(input);

		// If the input is empty just go back
		if( input.empty() ) continue;

		// Step through all known commands
		for( int i = 0; i <= QUIT && !handled; i++)
		{
			// Get the length of the current command to test
			int len = cmd[i].length();

			// See if the user put in input that matches the command
			if( equalIC(input.substr(0, len), cmd[i]) )
				// The command and input match, so process the command
				handled = executeCommand( static_cast<Commands>(i), input );
		}

		// The system did not recognize the command
		if( !handled ) cout << "Unknown Command...\n";
	}

    // User has finished, compress everything down and write it out to a file
	std::ofstream outfile{filename, std::ios::out | std::ios::binary};

    // Starting from the root node, recursively write each piece into the file
	root->writeToFile(outfile);

    // Jobs done!
	outfile.close();
}


/**
 *	\brief The Program's main entry point.  It accepts a commandline argument for the name of the filesystem.
 *	
 *	\param argc Number of arguments
 *	\param argv Array of arguments
 *	
 *	The program accepts a single commandline argument, the name of the filesystem on disk.  The system
 *	will attempt to find the specified FS and load it in, if none is found, a new one is created.  When
 *	the program is quit by the user, the new FS is written to disk.
 *	
 *	\return 0 if the program exited successfully, -1 otherwise.
 */
int main( int argc, char* argv[])
{
    // Verify we have enough arguments
	if( argc < 2)
	{
		cout << "Missing arguments, a filename is required." << endl;
		return -1;
	}
	
	// Attempt to load in the FS from disk
	std::fstream file(argv[1], std::ios::in | std::ios::binary);

	// If it was loaded successfully, use it as root
	if( file.good() )
	{
		file.close();
		commandLoop( argv[1], readInFile(argv[1]));
	}

	// If it wasn't loaded, then the system will make a new root
	else
	{
		file.close();
		commandLoop( argv[1], nullptr );
	}
	
	return 0;
}
