#pragma once

#include "IHack.h"

#include "CTargetHelper.h"
#include "CAimHelper.h"

class CAutoAirblast : public IHack
{
	//bool enabled;
	//bool breakMode;

	//var enabled_bool{"Enabled", &enabled};
	//var break_bool{"Break mode", &breakMode};

	F1_ConVar<Switch> *autoAirblastSwitch;
	F1_ConVar<bool> *enabled;
	F1_ConVar<bool> *aimMode;


	// std::unordered_map<int, float> targets;

	// maps entity indexes to their distance
	CDistanceTargetSystem targs;

	CSnapAimSystem aimer;

public:
	CAutoAirblast();

	// Inherited via IHack
	virtual const char *name() const override;
	virtual void processCommand(CUserCmd *pUserCmd) override;
	virtual bool processEntity(int index) override;

	void menuUpdate( F1_IConVar **menuArray, int &currIndex );

private:
	// int findBestTarget();
};
