#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>

class CFileManager
{

	std::vector<std::fstream> files;
public:
	typedef int fileHandle;

	using tokens = std::vector<std::pair<std::string, std::string>>;

	enum
	{
		invalidHandle = -1,
	};

	CFileManager();
	~CFileManager();

	// adds a file to the map
	fileHandle open(std::string path, unsigned int mode);

	// get a file from the map
	std::fstream &get(fileHandle h);

	// get name and token of files
	tokens CFileManager::TokenFile(fileHandle, char delim = ' ');
};
