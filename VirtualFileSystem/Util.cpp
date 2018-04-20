/*
 *	Andrew McGuiness
 *	ITEC 371 - Project 4
 *	4/19/2018
*/

#include "Util.h"


/**
 *	\brief	Compare two strings to see if they are equal, ignoring case.
 *	
 *	\param a First string to compare
 *	\param b Second string to compare
 *	
 *	\return True if both strings are equal ignoring case
 */
bool equalIC( const std::string& a, const std::string& b)
{
	bool same = true;

	// Verify the lengths
	if( a.length() != b.length())
		same = false;

	// Check each character for equality (ignoring case)
	else
		for( int i = 0; i < a.length() && same; i++)
			if(toupper(a[i]) != toupper(b[i])) 
				same = false;

	return same;
}
