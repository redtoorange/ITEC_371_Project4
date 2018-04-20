/*
 *	Andrew McGuiness
 *	ITEC 371 - Project 4
 *	4/19/2018
*/

#include "FSObject.h"

/**
 *	\return The name of this FileSystem Object
 */
const std::string& FSObject::getFileName() const
{
	return fileName;
}

/**
 *	\brief	Set the name of the file for the system.  This will be used
 *	when the object is serialized by the system.
 *	
 *	\param name The filename of this FSObject
 */
void FSObject::setFileName(const std::string& name)
{
	fileName = name;
}
