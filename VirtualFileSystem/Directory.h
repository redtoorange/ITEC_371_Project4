/*
 *	Andrew McGuiness
 *	ITEC 371 - Project 4
 *	4/19/2018
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

/**
*	\brief The core of the FileSystem is a complex structure of directories.
*	Directories are recursive in nature, that is, a directory can contain more
*	Directories.  All objects inside the Dir are stored in a vector of shared_ptr's.
*/
class Directory : public FSObject
{
public:
	/**
	 *	\brief Create a brand new Directory and return a shared pointer to it.
	 *	
	 *	\param name The name of this Directory
	 *	\param parent What Directory contains this Directory
	 *	\param root Is this Directory located at the root level?
	 *	
	 *	Create a new Directory for the File System.  If this is root, then it needs 
	 *	to be flagged so that it knows it has no parent.
	 *	
	 *	\return A shared pointer to the new Directory
	 */
	static std::shared_ptr<Directory> CreateDirectory( std::string name, Directory* parent, bool root = false);
	
	/**
	 *	\brief Validate the user entered filename to make sure it conforms to the spec.
	 *	
	 *	\param name The name entered by the user.
	 *	
	 *	The system will accept just a name as the name, or if there is an extension.
	 *	
	 *	\return True if the filename is valid.
	 */
	static bool validName( std::string* name);

	/**
	 *	\brief Constructor that should NOT be used.  Instead use the makeDirectory() function.
	 *	
	 *	\param name Name of the Directory
	 *	\param parent The Directory this contains this directory
	 *	\param root Is this Directory the root Directory?
	 *	
	 *	The makeDirectory() function should be used so that the new Directory is allocated on the heap.
	 *	This is required to allow for the filesystem to be serialized into a binary file.  If this 
	 *	constructor is used, the data will likely not be saved.
	 */
	Directory(const std::string& name, Directory* parent, bool root = false);

	/**
	 *	\brief Add another FSObject to this directory.
	 *	
	 *	\param obj The FSObject that should be added.
	 *	
	 *	A directory can contain any number of other FSObjects.
	 */
	void addObject( std::shared_ptr<FSObject> obj);

	/**
	 *	\brief Return the parent of this Directory or null if this is the root Directory.
	 *	
	 *	\return Get a pointer to the Directory this this Directory is located in.
	 */
	Directory* getParent();

	/**
	 *	\brief Print data for this Directory and print the data for all of it's children.
	 *	
	 *	\param tabs How many tabs to include before the file data (not used anymore)
	 */
	void printData(int tabs) override;
	
	/**
	 *	\brief Flatten this Directory and write it to a Binary File Stream.
	 *	
	 *	\param stream The Binary File Stream to write this Directory to.
	 *	
	 *	This Directy will be written to the stream, which in turn will recursively write all children
	 *	of this Directory to the stream.  Calling this on a root Directory will result in the entire
	 *	FileSystem being written to the file stream.
	 */
	void writeToFile(std::ofstream& stream) override;
	
	/**
	 *	\brief Print the directory to std out using the formating from the project spec
	 *	
	 *	\param os The output stream to write the data to
	 *	\param obj The Directory to write to std out
	 *	
	 *	\return The std out output stream
	 */
	friend std::ostream& operator<<(std::ostream& os, const Directory& obj)
	{
		if( obj.parent )
			os << *obj.parent << "/";

		return os << obj.fileName;
	}

	/**
	 *	\brief Search this directory for a directory that has a matching name.
	 *	
	 *	\param name The name of the Directory to locate.
	 *	
	 *	Only the first instance of a directory with a matching name is returned.  There
	 *	can be multiple Directories with the same name, but only the first can ever be used.
	 *	
	 *	\return A pointer to the Directory or null if not found.
	 */
	Directory* getDirectory(const std::string& name);

	/**
	 *	\brief Search this directory for a TextFile that has a matching name.
	 *	
	 *	\param name The name of the TextFile to locate.
	 *	
	 *	Only the first instance of a TextFile with a matching name is returned.  There
	 *	can be multiple TextFile with the same name, but only the first will ever be returned.
	 *	
	 *	\return A pointer to the TextFile or null if not found.
	 */
	TextFile* getTextfile(const std::string& name);

	/**
	 *	\brief Search this directory for a ProgramFile that has a matching name.
	 *	
	 *	\param name The name of the ProgramFile to locate.
	 *	
	 *	Only the first instance of a ProgramFile with a matching name is returned.  There
	 *	can be multiple ProgramFile with the same name, but only the first will ever be returned.
	 *	
	 *	\return A pointer to the ProgramFile or null if not found.
	 */
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