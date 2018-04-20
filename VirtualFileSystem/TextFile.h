/*
 *	Andrew McGuiness
 *	ITEC 371 - Project 4
 *	4/19/2018
*/

#ifndef TEXT_FILE_H
#define TEXT_FILE_H

#include <string>
#include <memory>
#include "File.h"


/**
 *	\brief A special type of File that is designed to hold a single string which is the "file contents"
 */
class TextFile : public File
{
public:
	/**
	 *	\brief Create a new TextFile from a given name.  The user will be prompted to enter the file contents.
	 *	
	 *	\param name The name of the TextFile that user gave.
	 *	
	 *	\return A shared pointer to the newly minted TextFile.
	 */
	static std::shared_ptr<TextFile> makeTextFile( const std::string& name );
	
	/**
	 *	\brief Inflate a TextFile from a flat binary file.
	 *	
	 *	\param name The name of the TextFile
	 *	\param stream The stream to read the contents of the file from.
	 *	
	 *	The TextFile can be loaded from a binary stream back into a full object on the heap.  This
	 *	is done using shared pointers to allow directories to manage the memory and reserialize the
	 *	file system when needed.
	 *	
	 *	\return A shared pointer to the new TextFile that should be handled by a Directory.
	 */
	static std::shared_ptr<TextFile> inflateTextFile(  std::string& name, std::ifstream& stream);
	
	/**
	 *	\brief Check to see if the name follows the rules for a TextFile as specified by the
	 *	Project.
	 *	
	 *	\param name User entered filename to validate.
	 *	
	 *	\return True if the filename meets the spec requirements.
	 */
	static bool validName( std::string* name);

	/**
	 *	\brief Constructor that should NOT be used.  Instead use the factory function makeTextFile().
	 *	
	 *	\param name The name of the TextFile
	 *	\param contents The body of the TextFile
	 *	
	 *	This constructor should NOT be used.  The system relies on the use of shared pointers to manage
	 *	the files contained in directories.  Using the constructor directly will violate this structure
	 *	and cause the file to not be serialize when the file system is saved.
	 */
	TextFile(const std::string& name, const std::string& contents);

	/**
	 *	\brief Formatted printer for TextFiles as per spec.
	 *	
	 *	\param tabs Number of tabs to include in the output.
	 */
	void printData(int tabs) override;
	
	/**
	 *	\brief Serialize the TextFile to a binary file steam.
	 *	
	 *	\param stream where to write the TextFile to.
	 */
	void writeToFile(std::ofstream& stream) override;

	/**
	 *	\brief Get the Text file's contents.  The body of the file that was
	 *	entered by the user at creation.
	 *	
	 *	\return The contents of the file as entered by the user.
	 */
	const std::string& getContents() const;
	
private:
	//! The text contents, of body, of this TextFile
	std::string fileContents;
};

#endif