/*
 *	Andrew McGuiness
 *	ITEC 371 - Project 3
 *	4/1/2018
*/

#ifndef FSOBJECT_H
#define FSOBJECT_H

#include <string>
#include <fstream>

/*!
*	\brief Abstract class the is the super class of all
*	File System objects, this includes Directories and
*	Files.
*/
class FSObject
{
public:
	FSObject() = default;

	//! Get the Objects FileName without an extension
	const std::string& getFileName() const;
	
	//! Set the Objects FileName
	void setFileName( const std::string& name);

	//! Print the Object to std out
	virtual void printData( int tabs) = 0;
	
	//! Write the Object to a stream
	virtual void writeToFile( std::ofstream& stream ) = 0;

protected:
	std::string fileName;
};

#endif