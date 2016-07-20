#pragma once

#include "IHack.h"
#include "SDK.h"
#include "network.h"
// TODO
// try to make all hacks singletons of some form
class CAntiSmac : public IHack
{

	std::vector<cvar_t> serverConvars;

	// custom set defaults for smac convar (when our automated one doesnt work!)
	CFileManager::tokens cvarList;

	F1_ConVar<Switch> *antiSmacSwitch;

	F1_ConVar<bool> *nameSpam;

public:
	CAntiSmac()
	{
		antiSmacSwitch = new F1_ConVar<Switch>( "Anti Smac", false );
		nameSpam = new F1_ConVar<bool>( " - Name spam", false );
	}

	static CAntiSmac *getInst()
	{
		static CAntiSmac inst;
		return &inst;
	}

	CAntiSmac(const CAntiSmac &c) = delete;
	void operator=(CAntiSmac &o) = delete;

	const char *name() const override;

	void init() override;

	void processCommand(CUserCmd *pUserCmd) override;

	bool processGetCvarValue(SVC_GetCvarValue *msg);

	bool processSetConVar(NET_SetConVar *msg);

	bool processStringCmd(NET_StringCmd *msg);

	void menuUpdate( F1_IConVar **menuArray, int &currIndex ) override;
};
