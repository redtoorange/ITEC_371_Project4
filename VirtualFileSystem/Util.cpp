/*
 *	Andrew McGuiness
 *	ITEC 371 - Project 3
 *	4/1/2018
*/

#include "Util.h"

//! Compare two strings, ignoring case, to see if they are equal.
bool equalIC( const std::string& a, const std::string& b)
{
	bool same = true;

	// Verify the lengths
	if( a.length() != b.length())
	{
		same = false;
	}

	// Check each character
	else
	{
		for( int i = 0; i < a.length() && same; i++)
		{
			if(toupper(a[i]) != toupper(b[i])) same = false;
		}
	}

	return same;
}
