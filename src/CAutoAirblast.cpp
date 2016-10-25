#include "CAutoAirblast.h"

#include "SDK.h"
#include "CEntity.h"
#include "Util.h"

CAutoAirblast gAutoAirblast;

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
	else
	{
		static bool oneTime = false;

		if(oneTime == false )
		{
			if(gTargetHelper.tryBecomeOwner(targetHelperKey) )
			{
				gTargetHelper.setValidTargetFn( targetHelperKey, [this]( int index ) -> bool { return this->isValidTarget( index ); } );
				gTargetHelper.setVisibleTargetFn( targetHelperKey, [this]( int index, Vector &t ) -> bool { return this->isVisibleTarget( index, t ); } );
				gTargetHelper.setCompareTarget( targetHelperKey, [ ]( const CTarget &b, const CTarget &n ) -> bool
				{
					// doesnt really matter as once one is within range, we will airblast
					if( __CTargetHelper::getDistanceToVector( b.target ) < __CTargetHelper::getDistanceToVector( n.target ) )
						return false;

					return true;
				} );

				oneTime = true;
			}
			else
			{
				Log::Msg( "Please disable the current module using the GlobalTargetSystem" );
				enabled->decrement();
			}
		}
	}

	if(gLocalPlayerVars.activeWeapon == classId::CTFFlameThrower)
	{
		CTarget closeIndex = gTargetHelper.getBestTarget();

		if(closeIndex.ent == -1)
			return;

		if(aimMode->getValue() == true)
		{
			//pUserCmd->viewangles.z += 23421341234151324123543514534534534590.0f;
			aimer.aim(pUserCmd, closeIndex.target, true);
		}

		pUserCmd->buttons |= IN_ATTACK2;
	}
	return;
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

bool CAutoAirblast::isValidTarget( int index )
{

	CBaseEntity *pBaseEntity = GetBaseEntity( index );

	if( pBaseEntity == NULL )
	{
		return false;
	}

	if( pBaseEntity->IsDormant() )
	{
		return false;
	}

	// if the projectile is from our own team we dont want or need to reflect it
	if( pBaseEntity->GetTeam() == gLocalPlayerVars.team )
	{
		return false;
	}

	auto entTag = CEntTag( pBaseEntity );

	Vector vel;

	if( entTag.isProjectile() )
	{
		return true;
	}

	return false;
}

bool CAutoAirblast::isVisibleTarget( int index, Vector &t )
{
	CBaseEntity *pBaseEntity = GetBaseEntity( index );

	Vector vel = EstimateAbsVelocity( pBaseEntity );

	//if( vel == Vector(0,0,0) )
	//{
	//	targs.removeTarget( index );
	//	return false;
	//}

	if( vel == Vector( 0, 0, 0 ) )
	{
		return false;
	}

	Vector origin;
	pBaseEntity->GetWorldSpaceCenter( origin );

	Vector eyePos = GetBaseEntity(me)->GetAbsOrigin() + gLocalPlayerVars.viewOffset;

	float latency = gInts.Engine->GetNetChannelInfo()->GetLatency( FLOW_INCOMING ) + gInts.Engine->GetNetChannelInfo()->GetLatency( FLOW_OUTGOING );

	Vector target = origin + ( vel * latency );

	//gInts.DebugOverlay->AddEntityTextOverlay( index, 0, 1, 255, 255, 255, 255, "%f", ::getDistanceToVector( target ) );

	float length = ( target - eyePos ).Length();

	if( length <= ( 185.0f ) )
	{
		//Log::Console( "Target at %f dist", length );
		return true;
	}

	return false;
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
