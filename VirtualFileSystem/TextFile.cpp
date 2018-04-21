/*
 *	Andrew McGuiness
 *	ITEC 371 - Project 4
 *	4/19/2018
*/

#include "TextFile.h"
#include <iostream>

/**
 *	\brief Create a new TextFile from a given name.  The user will be prompted 
 *	to enter the file contents.
 *	
 *	\param name The name of the TextFile that user gave.
 *	
 *	\return A shared pointer to the newly minted TextFile.
 */
std::shared_ptr<TextFile> TextFile::makeTextFile(const std::string& name)
{
	// Ensure the name is valid
	std::string n = name;
	if( !validName(&n) )
	{
		std::cout << "Invalid TextFile name!" << std::endl;
		return nullptr;
	}

	// Grab some contents for the file
	std::string contents;
	std::cout << "Enter file contents>";
	std::getline(std::cin, contents);

	// Return shared pointer to the TextFile
	return std::make_shared<TextFile>(n, contents);
}

/**
 *	\brief Inflate a TextFile from a flat binary file.
 *	
 *	\param name The name of the TextFile
 *	\param stream The stream to read the contents of the file from.
 *	
 *	The TextFile can be loaded from a binary stream back into a full object on 
 *	the heap.  This is done using shared pointers to allow directories to 
 *	manage the memory and re-serialize the file system when needed.
 *	
 *	\return A shared pointer to the new TextFile that should be handled by a 
 *	Directory.
 */
std::shared_ptr<TextFile> TextFile::inflateTextFile(std::string& name, 
	std::ifstream& stream)
{
	// Test the filename in-case of corruption
	std::string n = name;
	validName(&n);

	// Read in chars until we hit the end of the string
	char ch;
	std::string contents;
	while ((ch = stream.get()) != '\0') {
		contents += ch;
	}

	// Create a new TextFile
	return std::make_shared<TextFile>(n, contents);
}

/**
 *	\brief Check to see if the name follows the rules for a TextFile as 
 *	specified by the Project.
 *	
 *	\param name User entered filename to validate.
 *	
 *	\return True if the filename meets the spec requirements.
 */
bool TextFile::validName( std::string* name)
{
	// Strip the extension for storage
	auto back = name->end();
	if( *--back == 't' && *--back == '.')
	{
		name->erase(back, name->end());
	}

	// Check the length
	if( name->length() > 8)
	{
		std::cout << "TextFile names cannot exceed 8 characters.\n";
		return false;
	}
		
	// Verify all the characters are valid
	auto iter = name->begin();
	for(;iter != name->end(); ++iter)
	{
		// Validate all the characters
		char c = *iter;
		if( !(	(c >= 'a' && c <= 'z') || 
				(c >= 'A' && c <= 'Z') ||
				(c >= '0' && c <= '9')	)	)
		{
			// One has failed, notify the user.
			std::cout << "TextFile names must be alpha-numeric.\n";
			return false;
		}
	}

	return true;
}

/**
 *	\brief Constructor that should NOT be used.  Instead use the factory 
 *	function makeTextFile().
 *	
 *	\param name The name of the TextFile
 *	\param contents The body of the TextFile
 *	
 *	This constructor should NOT be used.  The system relies on the use of shared 
 *	pointers to manage the files contained in directories.  Using the 
 *	constructor directly will violate this structure and cause the file to not 
 *	be serialize when the file system is saved.
 */
TextFile::TextFile(const std::string& name, const std::string& contents)
{
	fileName = name;
	fileContents = contents;
}

/**
 *	\brief Formatted printer for TextFiles as per spec.
 *	
 *	\param tabs Number of tabs to include in the output.
 */
void TextFile::printData(int tabs)
{
	// Write data specific to text files
	std::cout << "Filename: " << fileName 
		<< ".t \tType: Text file" << std::endl;
}

/**
 *	\brief Serialize the TextFile to a binary file steam.
 *	
 *	\param stream where to write the TextFile to.
 */
void TextFile::writeToFile(std::ofstream& stream)
{
	// Write the filename to the stream
	stream << fileName;

	// Fill remaining spaces with null chars as per spec
	for( int i = fileName.length(); i < 8; i++)
	{
		stream << '\0';
	}

	// Write the file's extension to the stream
	stream << ".t";
	stream << '\0';

	// Write the contents to the stream
	stream << fileContents;
	stream << '\0';
}

/**
 *	\brief Get the Text file's contents.  The body of the file that was
 *	entered by the user at creation.
 *	
 *	\return The contents of the file as entered by the user.
 */
const std::string& TextFile::getContents() const
{
	return fileContents;
}
