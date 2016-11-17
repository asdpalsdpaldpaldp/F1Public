#include "SDK.h"
#include "Panels.h"
#include "unlinkpeb.h"

// maybe move these out of here and into their respective files
COffsets gOffsets;
CHack gHack;
CPlayerVariables gEntVars;
playerVars_t gLocalPlayerVars;

BOOL APIENTRY DllMain(HMODULE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	//If you manually map, make sure you setup this function properly.
	if(dwReason == DLL_PROCESS_ATTACH)
	{

		#ifndef ALT_BUILD
		//DisableThreadLibraryCalls(hInstance);
		UnlinkModuleFromPEB(hInstance);
		RemovePEHeader((DWORD)hInstance);
		#else
		#pragma message("ALT_BUILD ENABLED")
		#endif
		Log::Init(hInstance);

		setHandlers();

		// call our init
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&CHack::Init, hInstance, 0, 0);

	}
	return true;
}
