#pragma once

#include "IHack.h"
#include "SDK.h"
#include "network.h"
// TODO
// try to make all hacks singletons of some form
class CAntiSmac : public IHack<CAntiSmac>
{

	std::vector<cvar_t> serverConvars;

	// custom set defaults for smac convar (when our automated one doesnt work!)
	CFileManager::tokens cvarList;

	F1_ConVar<Switch> antiSmacSwitch = F1_ConVar<Switch>("AntiSmac", false);
	F1_ConVar<bool> nameSpam = F1_ConVar<bool>(" - Name spam", false);
	F1_ConVar<bool> debug = F1_ConVar<bool>(" - Debug info", false);

public:
	CAntiSmac()
	{
	}

	static CAntiSmac *getInst()
	{
		static CAntiSmac inst;
		return &inst;
	}

	CAntiSmac(const CAntiSmac &c) = delete;
	void operator=(CAntiSmac &o) = delete;

	const char *name() const;

	void init();

	void processCommand(CUserCmd *pUserCmd);

	bool processGetCvarValue(SVC_GetCvarValue *msg);

	bool processSetConVar(NET_SetConVar *msg);

	bool processStringCmd(NET_StringCmd *msg);

	void menuUpdate( F1_IConVar **menuArray, int &currIndex );
};