#pragma once

#include "IHack.h"

class CAntiaim : public IHack<CAntiaim>
{
	F1_ConVar<Switch> *AASwitch;

	F1_ConVar<bool> *enabled;
	F1_ConVar<bool> *fakeUp;
	F1_ConVar<bool> *fakeDown;
	F1_ConVar<bool> *faceBackwards;
	F1_ConVar<bool> *spinBot;
	F1_ConVar<int> *spinSpeed;
	F1_ConVar<bool> *jitter;
	F1_ConVar<bool> *lisp;

public:

	CAntiaim()
	{
		AASwitch = new F1_ConVar<Switch>( "AntiAim", false );
		enabled = new F1_ConVar<bool>( " - Enabled", false );
		fakeUp = new F1_ConVar<bool>( " - Fake up", false );
		fakeDown = new F1_ConVar<bool>( " - Fake down", false );
		faceBackwards = new F1_ConVar<bool>( " - Face backwards", false );
		spinBot = new F1_ConVar<bool>( " - Spinbot", false );
		spinSpeed = new F1_ConVar<int>( " - Spin speed", 60, 0, 1000, 5 );
		jitter = new F1_ConVar<bool>( " - Jitter", false );
		lisp = new F1_ConVar<bool>( " - Lisp", false );
	}

	// Inherited via IHack
	const char *name() const;
	void processCommand(CUserCmd *pUserCmd);
	void menuUpdate( F1_IConVar **menuArray, int &currIndex );
};

extern CAntiaim gAntiaim;