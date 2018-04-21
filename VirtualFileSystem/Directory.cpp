/*
 *	Andrew McGuiness
 *	ITEC 371 - Project 4
 *	4/19/2018
*/

#include "Directory.h"
#include "TextFile.h"

#include <iostream>
#include <string>

/**
 *	\brief Add another FSObject to this directory.
 *	
 *	\param obj The FSObject that should be added.
 *	
 *	A directory can contain any number of other FSObjects.
 */
void Directory::addObject( std::shared_ptr<FSObject> obj)
{
	objCount++;
	// Create a new shared pointer in the collection
	objects.emplace_back(obj);
}

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
std::shared_ptr<Directory> Directory::CreateDirectory( std::string name, 
	Directory* parent, bool root)
{
	// Ensure that the filename is valid according to the spec
	std::string n = name;
	if( !validName( &n ) )
	{
		std::cout << "Invalid directory name!" << std::endl;
		return nullptr;
	}

	// Toss back out new Dir for the FileSystem to use
	return std::make_shared<Directory>(n, parent, root);
}

/**
 *	\brief Validate the user entered filename to make sure it conforms to the 
 *	spec.
 *	
 *	\param name The name entered by the user.
 *	
 *	The system will accept just a name as the name, or if there is an extension.
 *	
 *	\return True if the filename is valid.
 */
bool Directory::validName( std::string* name)
{
	// Strip the extension if there is one
	auto back = name->end();
	if( *--back == 'd' && *--back == '.')
	{
		name->erase(back, name->end());
	}

	// Check the length is valid
	if( name->length() > 8)
	{
		std::cout << "Directory names cannot exceed 8 characters.\n";
		return false;
	}
		
	// Verify all the characters are valid
	auto iter = name->begin();
	for(;iter != name->end(); ++iter)
	{
		// Check each character
		char c = *iter;
		if( !((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) )
		{
			// One was invalid, notify the user
			std::cout 	<< "Directory names must contain only "
						<< "alphabetic characters.\n";
			return false;
		}
	}

	return true;
}

/**
 *	\brief Constructor that should NOT be used.  Instead use the makeDirectory() 
 *	function.
 *	
 *	\param name Name of the Directory
 *	\param parent The Directory this contains this directory
 *	\param root Is this Directory the root Directory?
 *	
 *	The makeDirectory() function should be used so that the new Directory is 
 *	allocated on the heap.  This is required to allow for the file system to be 
 *	serialized into a binary file.  If this constructor is used, the data will 
 *	likely not be saved.
 */
Directory::Directory(const std::string& name, Directory* parent, bool root)
{
	isRoot = root;
	fileName = name;
	this->parent = parent;
}

/**
 *	\brief Return the parent of this Directory or null if this is the root 
 *	Directory.
 *	
 *	\return Get a pointer to the Directory this this Directory is located in.
 */
Directory* Directory::getParent()
{
	return parent;
}

/**
 *	\brief Print data for this Directory and print the data for all of it's 
 *	children.
 *	
 *	\param tabs How many tabs to include before the file data (not used anymore)
 */
void Directory::printData(int tabs)
{		
	std::cout << "Directory Name: " << fileName << std::endl;
		
	// Recursively print kids
	for( auto& e : objects)
	{
		auto* d = dynamic_cast<Directory*>(e.get());
		if( d )
			std::cout << "Directory Name: " << d->getFileName() << std::endl;

		else
			e->printData(0);
	}
}

/**
 *	\brief Search this directory for a directory that has a matching name.
 *	
 *	\param name The name of the Directory to locate.
 *	
 *	Only the first instance of a directory with a matching name is returned.  
 *	There can be multiple Directories with the same name, but only the first 
 *	can ever be used.
 *	
 *	\return A pointer to the Directory or null if not found.
 */
Directory* Directory::getDirectory(const std::string& name)
{
	// Handle special case for going up a dir
	if( name == "..")
		return parent;
		
	// Search through all children
	for( auto& e : objects)
	{
		// Try to cast to a Dir, or nullptr if it fails
		Directory* d = dynamic_cast<Directory*>(e.get());

		// If the cast was valid, it's a Directory, check it's name
		if( d && d->getFileName() == name )
			return d;
	}

	// It was not found
	return nullptr;
}

/**
 *	\brief Search this directory for a TextFile that has a matching name.
 *	
 *	\param name The name of the TextFile to locate.
 *	
 *	Only the first instance of a TextFile with a matching name is returned.  
 *	There can be multiple TextFile with the same name, but only the first will 
 *	ever be returned.
 *	
 *	\return A pointer to the TextFile or null if not found.
 */
TextFile* Directory::getTextfile(const std::string& name)
{
	// Search through all children to find one with a matching name
	for( auto& e : objects)
	{
		// Try to cast to a TextFile
		TextFile* t = dynamic_cast<TextFile*>(e.get());

		// If the cast passes, check the name with and without an extension
		if( t && (t->getFileName() == name || name == t->getFileName() + ".t" ))
			return t;
	}

	return nullptr;
}

/**
 *	\brief Search this directory for a ProgramFile that has a matching name.
 *	
 *	\param name The name of the ProgramFile to locate.
 *	
 *	Only the first instance of a ProgramFile with a matching name is returned.  
 *	There can be multiple ProgramFile with the same name, but only the first 
 *	will ever be returned.
 *	
 *	\return A pointer to the ProgramFile or null if not found.
 */
ProgramFile* Directory::getProgramfile(const std::string& name)
{
	// Search through all children to find one with a matching name
	for( auto& e : objects)
	{
		// Try to cast to a ProgramFile, or nullptr if fails
		ProgramFile* p = dynamic_cast<ProgramFile*>(e.get());

		// If the cast passes, check the name with and without an extension
		if( p && (p->getFileName() == name || name == p->getFileName() + ".p" ))
			return p;
	}

	return nullptr;
}

/**
 *	\brief Flatten this Directory and write it to a Binary File Stream.
 *	
 *	\param stream The Binary File Stream to write this Directory to.
 *	
 *	This Directory will be written to the stream, which in turn will recursively 
 *	write all children of this Directory to the stream.  Calling this on a root 
 *	Directory will result in the entire FileSystem being written to the file 
 *	stream.
 */
void Directory::writeToFile(std::ofstream& stream)
{
	// Write the Directory's name
	stream << fileName;

	// Fill the rest of the spaces with null chars
	for( int i = fileName.length(); i < 8; i++)
		stream << '\0';

	// Add the extension as per spec.
	stream << ".d";
	stream << '\0';

	// Keep a track of how many objects are in the Directory
	stream.write((char*)&objCount, sizeof(objCount));
	

	// Recursively write all children to the stream
	for( auto& e : objects)
		e->writeToFile(stream);


	// Write the Dir Closer as per spec (no extension)
	stream << "end" + fileName;

	// Fill the rest with nulls
	for( int i = fileName.length(); i < 8; i++)
		stream << '\0';
}
