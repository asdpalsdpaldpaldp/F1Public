#pragma once

#include "IHack.h"

#include "CTargetHelper.h"
#include "CAimHelper.h"

class CAutoAirblast : public IHack<CAutoAirblast>
{
	//bool enabled;
	//bool breakMode;

	//var enabled_bool{"Enabled", &enabled};
	//var break_bool{"Break mode", &breakMode};

	F1_ConVar<Switch> *autoAirblastSwitch;
	F1_ConVar<bool> *enabled;
	F1_ConVar<bool> *aimMode;

	CSnapAimSystem aimer;

	DWORD targetHelperKey;

public:
	CAutoAirblast();

	// Inherited via IHack
	const char *name() const;
	void processCommand(CUserCmd *pUserCmd);

	void menuUpdate( F1_IConVar **menuArray, int &currIndex );

	bool isValidTarget( int index );
	bool isVisibleTarget( int index, Vector &t );

private:
	// int findBestTarget();
};

extern CAutoAirblast gAutoAirblast;
