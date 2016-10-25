#pragma once

#include "IHack.h"
#include "CEntity.h"

class CMisc : public IHack<CMisc>
{
	//bool bunnyhop;
	//bool tauntsilde;

	//var bunny_bool{ "Bunny hop", &bunnyhop };
	//var taunt_bool{ "Taunt slide", &tauntsilde };

	//VAR( bool, autoTaunt, "Auto taunt" );
	//VAR( int, fovChanger, "Fov", 10, 179, 110, 1 );

	F1_ConVar<Switch> *miscSwitch;
	F1_ConVar<bool> *bunnyHop;
	F1_ConVar<bool> *tauntSlide;
	F1_ConVar<int> *fovChanger;
	F1_ConVar<bool> *removeDisguise;
	F1_ConVar<bool> *alwaysAttack2;
	F1_ConVar<bool> *noPush;

public:
	CMisc()
	{
		miscSwitch = new F1_ConVar<Switch>( "Misc", false );
		bunnyHop = new F1_ConVar<bool>( " - Bunnyhop", false );
		tauntSlide = new F1_ConVar<bool>( " - Taunt slide", false );
		fovChanger = new F1_ConVar<int>( " - Fov changer", 110, 10, 179, 1 );
		removeDisguise = new F1_ConVar<bool>( " - Remove disguise", false );
		alwaysAttack2 = new F1_ConVar<bool>( " - Always attack2", false );
		noPush = new F1_ConVar<bool>( " - No push", false );
	}

	const char *name() const;
	void processCommandBeforePred(CUserCmd *pUserCmd);

	void processEntity( CBaseEntity *pBaseEntity );

	void menuUpdate( F1_IConVar **menuArray, int &currIndex );
};

extern CMisc gMisc;