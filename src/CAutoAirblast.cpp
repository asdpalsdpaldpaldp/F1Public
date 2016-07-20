#include "CAutoAirblast.h"

#include "SDK.h"
#include "CEntity.h"
#include "Util.h"

CAutoAirblast::CAutoAirblast()
{
	autoAirblastSwitch = new F1_ConVar<Switch>( "Auto Airblast", false );
	enabled = new F1_ConVar<bool>( " - Enabled" , false);
	aimMode = new F1_ConVar<bool>( " - Aim mode", false );
}

const char *CAutoAirblast::name() const { return "AUTO-AIRBLAST"; }

void CAutoAirblast::processCommand(CUserCmd *pUserCmd)
{
	if(!enabled->getValue())
		return;

	if(gLocalPlayerVars.activeWeapon == classId::CTFFlameThrower)
	{
		CTarget closeIndex = targs.getBestTarget();

		if(closeIndex.ent == -1)
			return;

		if(aimMode->getValue() == true)
		{
			//pUserCmd->viewangles.z += 23421341234151324123543514534534534590.0f;
			aimer.aim(pUserCmd, closeIndex.target, true);
		}

		pUserCmd->buttons |= IN_ATTACK2;

		targs.removeTarget( closeIndex.ent );
	}
	return;
}

bool CAutoAirblast::processEntity(int index)
{
	if(!enabled)
		return false;

	CEntity<> ent{index};

	if(ent.isNull())
	{
		targs.removeTarget(index);
		return false;
	}

	if(ent->IsDormant())
	{
		targs.removeTarget(index);
		return false;
	}

	// if the projectile is from our own team we dont want or need to reflect it
	if(ent.get<int>(gEntVars.iTeam) == gLocalPlayerVars.team)
	{
		targs.removeTarget(index);
		return false;
	}

	auto entTag = CEntTag(ent.castToPointer<CBaseEntity>());

	Vector vel;

	if( entTag.isProjectile() )
	{
		vel = EstimateAbsVelocity( ent.castToPointer<CBaseEntity>() );

		//if( vel == Vector(0,0,0) )
		//{
		//	targs.removeTarget( index );
		//	return false;
		//}
	}
	else
	{
		targs.removeTarget( index );
		return false;
	}

	Vector origin;
	ent->GetWorldSpaceCenter(origin);

	Vector eyePos = gLocalPlayerVars.pred.origin + CEntity<>{me}.get<Vector>(gEntVars.vecViewOffset);

	float latency = gInts.Engine->GetNetChannelInfo()->GetLatency(FLOW_INCOMING) + gInts.Engine->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING);

	Vector target = origin + (vel * latency);

	//gInts.DebugOverlay->AddEntityTextOverlay( index, 0, 1, 255, 255, 255, 255, "%f", ::getDistanceToVector( target ) );

	float length = ( target - eyePos ).Length();

	if( length <= ( 185.0f ) )
	{
		//Log::Console( "Target at %f dist", length );
		targs.addTarget( { index, (target - eyePos) } );
	}
	else
	{
	}

	return true;
}

void CAutoAirblast::menuUpdate( F1_IConVar **menuArray, int &currIndex )
{
	menuArray[ currIndex++ ] = autoAirblastSwitch;

	if(autoAirblastSwitch->getValue() == true )
	{
		menuArray[ currIndex++ ] = enabled;
		menuArray[ currIndex++ ] = aimMode;
	}
}

/*
int CAutoAirblast::findBestTarget()
{
	float bestDist = 8192.0f;
	int bestIndex = -1;

	for(auto &targ : targets)
	{
		if(targ.second < bestDist)
		{
			bestDist = targ.second;
			bestIndex = targ.first;
		}
	}

	return bestIndex;
}
*/
