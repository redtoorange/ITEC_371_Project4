/*
 *	Andrew McGuiness
 *	ITEC 371 - Project 3
 *	4/1/2018
*/

#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <vector>
#include <memory>
#include <ostream>

#include "FSObject.h"
#include "ProgramFile.h"

class File;
class TextFile;

/*!
*	\brief The core of the FileSystem is a complex structure of directories.
*	Directories are recursive in nature, that is, a directory can contain more
*	Directories.  All objects inside the Dir are stored in a vector of shared_ptr's.
*/
class Directory : public FSObject
{
public:
	//! Create a new Directory and return a shared pointer to it.
	static std::shared_ptr<Directory> CreateDirectory( std::string name, Directory* parent, bool root = false);
	
	//! Is the name valid for a Directory
	static bool validName( std::string* name);

	//! Constructor that should NOT be used
	Directory(const std::string& name, Directory* parent, bool root = false);

	//! Add an Object to this directory
	void addObject( std::shared_ptr<FSObject> obj);

	//! Return the parent of this Dir or null if this is root
	Directory* getParent();

	//! Print this Dir's data and recursively print children
	void printData(int tabs) override;
	
	//! Flatten this DIR and recursively flatten all Children
	void writeToFile(std::ofstream& stream) override;
	
	//! Print the directory to std out using the formmating from the requirements
	friend std::ostream& operator<<(std::ostream& os, const Directory& obj)
	{
		if( obj.parent )
			os << *obj.parent << "/";

		return os << obj.fileName;
	}

	//! Retrieve a sub direcotry from this directory by name.  Returns nullptr if nothing found.
	Directory* getDirectory(const std::string& name);

	//! Retrieve a textfile from this directory by name, the .t extension is optional. Returns nullptr if nothing found.
	TextFile* getTextfile(const std::string& name);

	ProgramFile* getProgramfile(const std::string& name);

private:
	//! All child objects of this DIR are stored here
	std::vector< std::shared_ptr<FSObject> > objects;
	
	//! The Directory that THIS Dir is stored in
	Directory* parent = nullptr;
	
	//! Flag this Dir if it's the root
	bool isRoot = false;

	//! Number of kids this Dir has
	int objCount = 0;
};
#endif