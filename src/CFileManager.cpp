#include "CFileManager.h"

CFileManager::CFileManager()
{
}

CFileManager::~CFileManager()
{ // destruct file handles
}

CFileManager::tokens CFileManager::TokenFile(fileHandle h, char delim)
{
	std::vector<std::string> lines;

	tokens ret;

	std::pair<std::string, std::string> strPair;

	{
		std::string line;

		while(std::getline(files[h], line))
		{
			lines.push_back(line);
		}
	}

	for(auto &line : lines)
	{
		int pos = 0, start = 0, time = 0;

		for(auto &ch : line)
		{
			if(ch == delim)
			{
				std::string temp = "";

				// worlds most hackiest hack
				if(time == 0)
				{
					for(int i = start; i < pos; i++)
					{
						temp += line[i];
					}
					strPair.first = temp;
				}
				else
				{
					for(int i = start; i <= pos; i++)
					{
						temp += line[i];
					}
					strPair.second = temp;
				}

				start = pos + 1;
				time++;
			}
			else
				pos++;
		}
		ret.push_back(strPair);
	}

	return ret;
}

CFileManager::fileHandle CFileManager::open(std::string path, unsigned int mode)
{
	std::fstream file;

	file.open(path, mode);

	// if it doesnt open, return an invalid handle
	if(!file.is_open())
		return invalidHandle;

	files.push_back(std::move(file));

	// a fileHandle is just the position in the vector
	return files.size() - 1;
}

std::fstream &CFileManager::get(fileHandle h)
{
	return files[h];
}

CFileManager gFileManager;
