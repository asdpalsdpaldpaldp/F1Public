#pragma once

#include "IHack.h"

class CNoise : public IHack<CNoise>
{

	//bool enabled;

	//var enabled_bool{ "Enabled", &enabled };

	F1_ConVar<bool> *enabled;

public:
	CNoise()
	{
		enabled = new F1_ConVar<bool>( "Noisemaker Spam", false );
	}

	// Inherited via IHack
	const char *name() const;
	void processCommand(CUserCmd *pUserCmd);

	void menuUpdate( F1_IConVar **menuArray, int &currIndex );

};

extern CNoise gNoise;