#pragma once

#include "IHack.h"
#include "CEntity.h"

class CBaseCombatWeapon;

class CBackstab : public IHack<CBackstab>
{
	F1_ConVar<Switch> *autoBackstabSwitch;
	F1_ConVar<bool> *enabled;

public:
	CBackstab()
	{
		autoBackstabSwitch = new F1_ConVar<Switch>( "Auto backstab", false );
		enabled = new F1_ConVar<bool>( " - Enabled", false );
	}

	const char *name() const;
	void processCommandBeforePred(CUserCmd *pUserCmd);

	void menuUpdate( F1_IConVar **menuArray, int &currIndex );

private:
	// internal
	// replaces netvar
	bool canBackstab( CBaseCombatWeapon *pBaseCombatWeapon, CBaseEntity *pBaseEntity );

	bool isBehind( CBaseEntity *pBaseEntity, CBaseEntity *pLocalEntity );

	bool engineCanBackstab(CBaseCombatWeapon *weapon, CBaseEntity *target);

	bool predicts(CEntity<> &local, CEntity<> &other);
};

extern CBackstab gBackstab;