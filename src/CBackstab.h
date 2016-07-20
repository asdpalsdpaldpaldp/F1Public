#pragma once

#include "IHack.h"
#include "CEntity.h"

class CBaseCombatWeapon;

class CBackstab : public IHack
{
	F1_ConVar<Switch> *autoBackstabSwitch;
	F1_ConVar<bool> *enabled;

public:
	CBackstab()
	{
		autoBackstabSwitch = new F1_ConVar<Switch>( "Auto backstab", false );
		enabled = new F1_ConVar<bool>( " - Enabled", false );
	}

	const char *name() const override;
	void processCommand(CUserCmd *pUserCmd) override;

private:
	// internal
	// replaces netvar
	bool canBackstab(CEntity<CBaseCombatWeapon> &weap_entity, CEntity<> &local_entity);

	bool isBehind(CEntity<> &other_entity, CEntity<> &local_entity);

	bool engineCanBackstab(CBaseCombatWeapon *weapon, CBaseEntity *target);

	bool predicts(CEntity<> &local, CEntity<> &other);
};
