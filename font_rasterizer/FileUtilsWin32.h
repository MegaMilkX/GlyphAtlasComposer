#ifndef _FILEUTILSWIN32_H_
#define _FILEUTILSWIN32_H_

#include <windows.h>
#include <string>

std::vector<std::string> ListFiles(std::string name)
{
	std::vector<std::string> filenames;

	LPWIN32_FIND_DATA findData;
	HANDLE h = FindFirstFile(name.c_str(), findData);

	if(h != INVALID_HANDLE_VALUE)
	{
		filenames.insert(filenames.end(), std::string(findData->cFileName));
		while(FindNextFile(h, findData))
		{
			filenames.insert(filenames.end(), std::string(findData->cFileName));
		}
	}

	return filenames;
}

#endif