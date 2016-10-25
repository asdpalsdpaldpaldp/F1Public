#include "error.h"
#include <iostream>
#include <cstdlib>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

void exitWithError (std::string error)
{
	//std::cout << error;

	MessageBox(NULL, error.c_str(), NULL, NULL);

	exit(1);
}

void exitWithError()
{
	exitWithError("FAIL\nAborting...");
}
