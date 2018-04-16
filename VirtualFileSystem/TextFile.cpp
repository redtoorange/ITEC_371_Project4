/*
 *	Andrew McGuiness
 *	ITEC 371 - Project 3
 *	4/1/2018
*/

#include "TextFile.h"

#include <iostream>

std::shared_ptr<TextFile> TextFile::makeTextFile(const std::string& name)
{
	// Check the name
	std::string n = name;
	if( !validName(&n) )
	{
		std::cout << "Invalid textfile name!" << std::endl;
		return nullptr;
	}

	// Grab some contents for the file
	std::string contents;
	std::cout << "Enter file contents>";
	std::getline(std::cin, contents);

	// Return a fresh TextFile
	return std::make_shared<TextFile>(n, contents);
}

std::shared_ptr<TextFile> TextFile::inflateTextFile(std::string& name, std::ifstream& stream)
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
		std::cout << "File names cannot exceed 8 characters.\n";
		return false;
	}
		
	// Verify all the characters are valid
	auto iter = name->begin();
	for(;iter != name->end(); ++iter)
	{
		char c = *iter;
		if( !((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||((c >= '0' && c <= '9')) ) )
		{
			std::cout << "File names must be alpha-numeric.\n";
			return false;
		}
	}

	return true;
}


// Doesn't do much, don't use!
TextFile::TextFile(const std::string& name, const std::string& contents)
{
	fileName = name;
	fileContents = contents;
}


void TextFile::printData(int tabs)
{
	// Write data specific to text files
	std::cout << "Filename: " << fileName << ".t \tType: Text file" << std::endl;
}

void TextFile::writeToFile(std::ofstream& stream)
{
	// Followed the project guidelines for the exact format
	stream << fileName;
	for( int i = fileName.length(); i < 8; i++)
	{
		stream << '\0';
	}
	stream << ".t";
	stream << '\0';

	stream << fileContents;
	stream << '\0';
}

const std::string& TextFile::getContents() const
{
	return fileContents;
}
