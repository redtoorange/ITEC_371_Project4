/*
 *	Andrew McGuiness
 *	ITEC 371 - Project 4
 *	4/19/2018
*/

#ifndef UTIL_H
#define UTIL_H

#include <string>

/**
 *	The Util file contains data for usage by the program to handle different 
 *	kinds of input.  All the commands the system is required to use are listed 
 *	here.
 */

//! Contains the string literal version of all commands
const std::string cmd[] = {
	"pwd",
	"ls",
	"mkdir",
	"cat",
	"createTextfile",
	"run",
	"start",
	"step",
	"cd",
	"addProgram",
	"getmemory",
	"setmemory",
	"setburst",
	"getburst",
	"quit"
};

//! Contains the enumerated version of all commands.
enum Commands : int{
	PWD 		= 0,
	LIST 		= 1,
	MKDIR 		= 2,
	CAT 		= 3,
	CREATE_TEXT	= 4,
	RUN 		= 5,
	START 		= 6,
	STEP 		= 7,
	CD 			= 8,
	ADD_PRO 	= 9,
	GET_MEM 	= 10,
	SET_MEM 	= 11,
	SET_BURST 	= 12,
	GET_BURST 	= 13,
	QUIT 	  	= 14
	
};

/**
 *	\brief	Compare two strings to see if they are equal, ignoring case.
 *	
 *	\param a First string to compare
 *	\param b Second string to compare
 *	
 *	\return True if both strings are equal ignoring case
 */
bool equalIC( const std::string& a, const std::string& b);

#endif