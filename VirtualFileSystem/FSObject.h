/*
 *	Andrew McGuiness
 *	ITEC 371 - Project 4
 *	4/19/2018
*/

#ifndef FSOBJECT_H
#define FSOBJECT_H

#include <string>
#include <fstream>

/**
 *	\brief Abstract class the is the super class of all File System objects, 
 *	this includes Directories and Files.
 */
class FSObject
{
public:
	FSObject() = default;

	/**
	 *	\return The name of this FileSystem Object
	 */
	const std::string& getFileName() const;
	
	/**
	 *	\brief	Set the name of the file for the system.  This will be used
	 *	when the object is serialized by the system.
	 *	
	 *	\param name The filename of this FSObject
	 */
	void setFileName( const std::string& name);

	/**
	 *	\brief	All FSObjects must have a way of displaying their internal data.
	 *	
	 *	\param tabs How many tabs to include in the output.
	 *	
	 *	This function is used so that each FSObject can have it's own way of 
	 *	responding to the ls command.
	 */
	virtual void printData( int tabs) = 0;
	
	/**
	 *	\brief All FSObjects need to have a way of writing them to a binary 
	 *	File.
	 *	
	 *	\param stream Where to write the FSObject to.
	 *	
	 *	All FSObjects must implement this function.  The point is to have a 
	 *	recursive way of serializing the entire FileSystem.
	 */
	virtual void writeToFile( std::ofstream& stream ) = 0;

protected:
	//! The name of this FSObject
	std::string fileName;
};

#endif