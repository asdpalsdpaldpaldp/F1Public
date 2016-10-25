// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include "resource.h"

bool extractResource(const HINSTANCE hInstance, WORD resourceID, LPCTSTR szFilename)
{
	bool bSuccess = false;
	try
	{
		// Find and load the resource
		HRSRC hResource = FindResource(hInstance, MAKEINTRESOURCE(resourceID), L"DLL");
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
		MessageBox(NULL, L"Failed to load dll!", NULL, NULL);
	}
	return bSuccess;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch(ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		extractResource(hModule, IDR_DLL1, L"never-aim.dll");
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		FreeLibraryAndExitThread(hModule, 0);
		break;
	}
	return TRUE;
}

