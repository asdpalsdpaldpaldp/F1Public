#include "CBackstab.h"

#include "SDK.h"
#include "CEntity.h"
#include "Trace.h"
#include "Util.h"

CBackstab gBackstab;

const char *CBackstab::name() const { return "AUTO-BACKSTAB"; }

void CBackstab::processCommandBeforePred(CUserCmd *pUserCmd)
{
	// if the hack is not enabled
	if(!enabled->getValue())
		return;

	CBaseEntity *pLocalEntity = GetBaseEntity( me );

	if(pLocalEntity == nullptr)
		return;

	CBaseCombatWeapon *pBaseCombatWeapon =  (CBaseCombatWeapon *)pLocalEntity->GetActiveWeapon();

	if( pBaseCombatWeapon == nullptr) // is not null
		return;

	if( !CEntTag{ pBaseCombatWeapon }.isWeap() )
		return;

	//if(!weapon.get<bool>(gEntVars.bReadyToBackstab)) // is ready to backstab
	//	return false;

	//char *name = pBaseCombatWeapon->GetName();

	if(/*name[10] == 'k' && name[11] == 'n'*/ pBaseCombatWeapon->GetClientClass()->iClassID == classId::CTFKnife) // check the important characters - there is only one weapon with a [k] and an [n] at pos 10 and 11
	{
		// TODO fixme

		//gInts.LagCompensation->StartLagCompensation( localPlayer.castToPointer<CBaseEntity>(), pUserCmd );

		if(!canBackstab( pBaseCombatWeapon, pLocalEntity))
			return;

		//gInts.LagCompensation->FinishLagCompensation( localPlayer.castToPointer<CBaseEntity>() );

		pUserCmd->buttons |= IN_ATTACK;
	}
	else
	{
		return;
	}


	return;
}

void CBackstab::menuUpdate( F1_IConVar ** menuArray, int & currIndex )
{
	menuArray[ currIndex++ ] = autoBackstabSwitch;
	if( autoBackstabSwitch->getValue() )
	{
		menuArray[ currIndex++ ] = enabled;
	}
}

// backstab helper
bool CBackstab::canBackstab( CBaseCombatWeapon *pBaseCombatWeapon, CBaseEntity *pBaseEntity )
{
	trace_t trace;

	CTFBaseWeaponMelee *pBaseMeleeWeapon = CTFBaseWeaponMelee::FromBaseEntity(pBaseCombatWeapon);

	bool istrace = pBaseMeleeWeapon->DoSwingTrace(trace);

	if(!istrace)
		return false;

	if(!trace.m_pEnt)
		return false;

	if(trace.m_pEnt->IsDormant())
		return false;

	//CEntity<> other_entity(trace.m_pEnt->GetIndex());

	if(trace.m_pEnt->IsAlive() == false)
		return false;

	classId Class = trace.m_pEnt->GetClientClass()->iClassID;

	if(Class != classId::CTFPlayer)
		return false;

	int other_team = trace.m_pEnt->GetTeam(); // so we dont have to get the netvar every time

	if(other_team == gLocalPlayerVars.team || (other_team < 2 || other_team > 3)) // check team is not our team or invalid team
		return false;

	if(isBehind(trace.m_pEnt, pBaseEntity))
	{
		//Log::Console("Can Backstab!");
		return true;
	}

	return false;
}

bool CBackstab::isBehind( CBaseEntity *pBaseEntity, CBaseEntity *pLocalEntity )
{
	if( pBaseEntity == nullptr )
		return false;

	if( pLocalEntity == nullptr )
		return false;

	// Get the forward view vector of the target, ignore Z
	Vector vecVictimForward;
	AngleVectors(pBaseEntity->GetPrevLocalAngles(), &vecVictimForward);
	vecVictimForward.z = 0.0f;
	vecVictimForward.NormalizeInPlace();

	// Get a vector from my origin to my targets origin
	Vector vecToTarget;
	Vector localWorldSpace;
	pLocalEntity->GetWorldSpaceCenter(localWorldSpace);
	Vector otherWorldSpace;
	pBaseEntity->GetWorldSpaceCenter(otherWorldSpace);
	vecToTarget = otherWorldSpace - localWorldSpace;
	vecToTarget.z = 0.0f;
	vecToTarget.NormalizeInPlace();

	// Get a forward vector of the attacker.
	Vector vecOwnerForward;
	AngleVectors(pLocalEntity->GetPrevLocalAngles(), &vecOwnerForward);
	vecOwnerForward.z = 0.0f;
	vecOwnerForward.NormalizeInPlace();

	float flDotOwner = vecOwnerForward.Dot(vecToTarget);
	float flDotVictim = vecVictimForward.Dot(vecToTarget);

	// Make sure they're actually facing the target.
	// This needs to be done because lag compensation can place target slightly behind the attacker.
	if(flDotOwner > 0.5)
		return (flDotVictim > -0.1);
	
	return false;

	//typedef bool(__thiscall * IsBehindFn)(CBaseCombatWeapon *, CBaseEntity *);

	//static DWORD dwLoc = gSignatures.GetClientSignature("E8 ? ? ? ? 84 C0 74 08 5F B0 01 5E 5D C2 04 00 A1") + 0x1;

	//static DWORD dwIsBehind = ((*(PDWORD)(dwLoc)) + dwLoc + 4);

	//static IsBehindFn isBehind = (IsBehindFn)dwIsBehind;

	//return isBehind(local_entity.castToPointer<CBaseCombatWeapon>(), other_entity.castToPointer<CBaseEntity>());
}

bool CBackstab::engineCanBackstab(CBaseCombatWeapon *weapon, CBaseEntity *target)
{
	typedef bool(__thiscall * BackstabFn)(CBaseCombatWeapon *, CBaseEntity *);

	static DWORD dwBackstabLoc = gSignatures.GetClientSignature("E8 ? ? ? ? 84 C0 74 36 80 BF") + 0x1;

	static DWORD dwBackstab = ((*(PDWORD)(dwBackstabLoc)) + dwBackstabLoc + 4);

	static BackstabFn backstab = (BackstabFn)dwBackstab;

	if(!weapon || !target)
		return false;

	CUtilMove::safeRunSimulation(gInts.Prediction.get(), target);

	bool r = backstab(weapon, target);

	return r;
}

bool CBackstab::predicts(CEntity<> &local, CEntity<> &other) { return false; }
