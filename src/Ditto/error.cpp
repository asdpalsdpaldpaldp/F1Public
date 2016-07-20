#include "error.h"
#include <iostream>
#include <cstdlib>

void exitWithError (std::string error)
{
	std::cout << error;

	system("pause");
	exit(1);
}

void exitWithError()
{
	exitWithError("FAIL\nAborting...");
}
