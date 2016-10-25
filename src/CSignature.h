#pragma once

#include "baseHeaders.h"

class CSignature
{
public:
	DWORD dwFindPattern(DWORD dwAddress, DWORD dwLength, const char *szPattern);
	HMODULE GetModuleHandleSafe(const char *pszModuleName);
	DWORD GetClientSignature(char *chPattern);
	DWORD GetEngineSignature(char *chPattern);
	DWORD GetServerSignature( char *chPattern );
};

extern CSignature gSignatures;
