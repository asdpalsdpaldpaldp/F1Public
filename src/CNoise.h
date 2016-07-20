#pragma once

#include "IHack.h"

class CNoise : public IHack
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
	virtual const char *name() const override;
	virtual void processCommand(CUserCmd *pUserCmd) override;

	void menuUpdate( F1_IConVar **menuArray, int &currIndex ) override;

};