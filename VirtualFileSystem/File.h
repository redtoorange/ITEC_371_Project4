/*
 *	Andrew McGuiness
 *	ITEC 371 - Project 4
 *	4/19/2018
*/

#ifndef FILE_H
#define FILE_H

#include "FSObject.h"

/**
 *	\brief Super class of all File type FSO's.  Will be used in the future to 
 *	handle common functions.
 */
class File : public FSObject
{
public:
	virtual ~File() = default;
};

#endif