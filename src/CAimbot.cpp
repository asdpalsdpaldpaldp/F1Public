#include "CAimbot.h"
#include "SDK.h"
#include "CEntity.h"
#include "trace.h"
#include "Util.h"
#include "CDrawManager.h"
#include "CAimHelper.h"

#include "projectile.h"
#include "F1_Cache.h"
#include "CPlayerManager.h"

CAimbot gAimbot;

// simple define for sniper zoom
#define ZOOM_BASE_DAMAGE 150.0f
#define SNIPERRIFLE_BASE_DAMANGE 50.0f

const char *CAimbot::name() const { return "AIMBOT"; }

void CAimbot::processCommand(CUserCmd *pUserCmd)
{
	_TRY
	{
		static bool oneTime = false;
		static targetHelpers oldHelper = (targetHelpers)-1;

		if(enabled->getValue())
		{
			static int lastTick = 0;

			if(!oneTime)
			{
				if(gTargetHelper.tryBecomeOwner(targetSystemKey))
				{
					gTargetHelper.setValidTargetFn(targetSystemKey, [](CBaseEntity *ent) -> bool { return gAimbot.isValidTarget(ent); });
					gTargetHelper.setVisibleTargetFn(targetSystemKey, [](CBaseEntity *ent, Vector &t) -> bool { return gAimbot.isVisibleTarget(ent, t); });
					gTargetHelper.setCompareLocationFn(targetSystemKey, [](CBaseEntity *ent) -> Vector { return ent->GetAbsOrigin(); });
					oneTime = true;
				}
				else
				{
					Log::Msg("Please disable the current module using the GlobalTargetSystem");
					enabled->decrement();
				}
			}

			if(oldHelper != tSystem->getValue())
			{
				if( tSystem->getValue() == targetHelpers::distance )
					gTargetHelper.setCompareTargetFn( targetSystemKey,
						[]( const CTarget &b, const CTarget &n ) -> bool
				{
					if(__CTargetHelper::getDistanceToVector(b.target) < __CTargetHelper::getDistanceToVector(n.target))
						return false;

					return true;
				} );
				else if( tSystem->getValue() == targetHelpers::fov )
					gTargetHelper.setCompareTargetFn( targetSystemKey,
						[]( const CTarget &b, const CTarget &n ) -> bool
				{
					if(__CTargetHelper::GetFovFromLocalPlayer(b.target) < __CTargetHelper::GetFovFromLocalPlayer(n.target))
						return false;

					return true;
				});

				oldHelper = tSystem->getValue();
			}

			oldAngles = pUserCmd->viewangles;

			// this does the silent movement fix
			aim(pUserCmd);

			return;
		}
		else
		{
			oneTime = false;
			gTargetHelper.releaseOwner(targetSystemKey);
		}

		return;
	}
		_CATCH
	{
		//targetSystem = switchTargetSystem( targetSystem, tSystem->getValue() );
	}
}

bool CAimbot::paint()
{
	if(!enabled->getValue())
		return false;

	//gDrawManager.DrawString( "hud", 0, 600, COLOR_OBJ, "Lag: %f", GetActualLerpDiff() );

	// draw the current fov
	if(tSystem->getValue() == targetHelpers::fov)
	{
		// TODO its either 16/9 or 9/16
		float actualFov = 2 * atan(( 16.0f / 9.0f ) * ( 3.0f / 4.0f ) * tan(DEG2RAD(GetBaseEntity(me)->GetFov() / 2.0f)));

		float radius = tan(DEG2RAD(fovLimit->getValue() / 2)) / tan(DEG2RAD(actualFov / 2));

		gDrawManager.DrawCircle(gScreenSize.iScreenWidth / 2, gScreenSize.iScreenHeight / 2, radius, 20, COLORCODE(255, 255, 255, 255));
	}

	if(!projectilePrediction->getValue())
		return false;

	//int y = 500;
	//gDrawManager.DrawString( "hud", 0, y, COLOR_OBJ, "time: %f", time );
	//y += gDrawManager.GetHudHeight();
	//gDrawManager.DrawString( "hud", 0, y, COLOR_OBJ, "asymetric time: %f", asymTime );
	//y += gDrawManager.GetHudHeight();
	//gDrawManager.DrawString( "hud", 0, y, COLOR_OBJ, "speed: %f", speed );
	//y += gDrawManager.GetHudHeight();
	//gDrawManager.DrawString( "hud", 0, y, COLOR_OBJ, "dist: %f", distance );
	//y += gDrawManager.GetHudHeight();

	//projectileHelper::PredictPath( targetSystem->getLastBestTarget().ent );

	Vector vTargetScreen;

	//if(gDrawManager.WorldToScreen(predPos, vTargetScreen) )
	//{
	//	gDrawManager.DrawRect( vTargetScreen.x, vTargetScreen.y, 16, 16, gDrawManager.dwGetTeamColor( 2 ) );
	//}

	return true;
}

void CAimbot::menuUpdate(F1_IConVar ** menuArray, int & currIndex)
{
	menuArray[currIndex++] = aimbotSwitch;

	if(aimbotSwitch->getValue())
	{
		menuArray[currIndex++] = enabled;
		menuArray[currIndex++] = aimMelee;
		menuArray[currIndex++] = autoHitbox;
		menuArray[currIndex++] = hitbox;
		menuArray[currIndex++] = zooomOnly;
		menuArray[currIndex++] = waitForCharge;
		menuArray[currIndex++] = ignoreFriends;
		menuArray[currIndex++] = ignoreCond;
		//menuArray[ currIndex++ ] = latencyPrediction;
		menuArray[currIndex++] = autoShoot;
		menuArray[currIndex++] = aimOnClick;
		menuArray[currIndex++] = projectilePrediction;
		if(projectilePrediction->getValue() == true)
			menuArray[currIndex++] = projectileQuality;
		menuArray[currIndex++] = predMode;
		menuArray[currIndex++] = tSystem;
		menuArray[currIndex++] = fovLimit;
		menuArray[currIndex++] = useSilent;
	}
}

//float CAimbot::getDistance(CEntity<> &ent)
//{
//
//	if(ent.isNull())
//		return 8192.0f;
//
//	return getDistanceToVector(ent->GetAbsOrigin());
//}
//
inline float CAimbot::getDistanceToVector(Vector v)
{
	CBaseEntity *pLocal = gInts->EntList->GetClientEntity(me);

	if(!pLocal)
		return 8192.0f;

	return pLocal->GetAbsOrigin().DistToSqr(v);
}

bool CAimbot::visible(CBaseEntity *pBaseEntity)
{
	trace_t trace;
	Ray_t ray;
	CBaseFilter filter;

	//CEntity<> local{ me };
	CBaseEntity *pLocalEntity = GetBaseEntity(me);

	filter.SetIgnoreEntity(pLocalEntity);

	// the get hitbox already attempts to predict it ( if that is enabled )
	Vector hit = getHitBoxVector(pBaseEntity, findBestHitbox(pBaseEntity));

	// use the predicted origin the view offset is insignificant
	ray.Init(pLocalEntity->GetViewPos(), hit);

	gInts->EngineTrace->TraceRay(ray, MASK_AIMBOT | CONTENTS_HITBOX, &filter, &trace);

	if(trace.m_pEnt)
	{
		return ( trace.m_pEnt == pBaseEntity && trace.hitGroup != 0 );
	}

	return true;
}

bool CAimbot::visible(CBaseEntity *pBaseEntity, int hitbox)
{
	trace_t trace;
	Ray_t ray;
	CBaseFilter filter;

	//CEntity<> local{ me };
	CBaseEntity *pLocalEntity = GetBaseEntity(me);

	filter.SetIgnoreEntity(pLocalEntity);

	// the get hitbox already attempts to predict it ( if that is enabled )
	Vector hit = getHitBoxVector(pBaseEntity, hitbox);

	// use the predicted origin the view offset is insignificant
	ray.Init(pLocalEntity->GetViewPos(), hit);

	gInts->EngineTrace->TraceRay(ray, MASK_AIMBOT | CONTENTS_HITBOX, &filter, &trace);

	if(trace.m_pEnt)
	{
		return ( trace.m_pEnt == pBaseEntity && trace.hitGroup != 0 );
	}

	return true;
}

bool CAimbot::visible(CBaseEntity *pBaseEntity, Vector v)
{
	trace_t trace;
	Ray_t ray;
	CBaseFilter filter;

	//CEntity<> local{ me };
	CBaseEntity *pLocalEntity = GetBaseEntity(me);

	filter.SetIgnoreEntity(pLocalEntity);

	// use the predicted origin the view offset is insignificant
	ray.Init(pLocalEntity->GetAbsOrigin() + gLocalPlayerVars.viewOffset, v);

	gInts->EngineTrace->TraceRay(ray, MASK_AIMBOT | CONTENTS_HITBOX, &filter, &trace);

	if(trace.m_pEnt)
	{
		return ( trace.m_pEnt == pBaseEntity && trace.hitGroup != 0 );
	}

	return true;
}

void CAimbot::aim(CUserCmd *pUserCmd)
{
	Vector angles;

	// TODO change compareTargetFn somewhere instead
	//if( targetSystem->getType() != tSystem->getValue() )
	//{
	//	targetSystem = switchTargetSystem( targetSystem, tSystem->getValue() );
	//}

	CTarget target = gTargetHelper.getBestTarget();

	if(target.ent == -1)
	{
		valid = false;
		return;
	}

	//switch( tSystem->getValue() )
	//{
	//case targetHelpers::distance:
	//	if( targetSystem->getBestStat() == FLT_MAX )
	//		return;
	//	break;
	//case targetHelpers::fov:
	//	if( targetSystem->getBestStat() > fovLimit->getValue() )
	//		return;
	//	break;
	//default:
	//	break;
	//}

	//CEntity<> local{ me };
	CBaseEntity *pLocalEntity = GetBaseEntity(me);

	if(pLocalEntity == NULL)
		return;

	if(!aimMelee->getValue())
	{
		// if we have a melee weapon dont aim
		if(gLocalPlayerVars.wepTag.isMelee() || gLocalPlayerVars.wepTag.isPda())
			return;
	}

	//CEntity<> other{ target.ent };
	CBaseEntity *pTargetEntity = GetBaseEntity(target.ent);
	if(pTargetEntity == NULL)
		return;

	//CEntity<> localWeapon{ HANDLE2INDEX( local.get<int>( gEntVars.hActiveWeapon ) ) };
	CBaseCombatWeapon *pBaseLocalWeapon = pLocalEntity->GetActiveWeapon();
	if(pBaseLocalWeapon == NULL)
		return;

	// if we manually attack, ALWAYS aim and attack
	if(!( pUserCmd->buttons & IN_ATTACK ))
	{
		if(aimOnClick->getValue())
			return;

		if(gLocalPlayerVars.Class == tf_classes::TF2_Sniper)
		{
			if(gLocalPlayerVars.cond & tf_cond::TFCond_Zoomed)
			{
				// if we cant get them with the no charge
				if(pTargetEntity->GetHealth() > SNIPERRIFLE_BASE_DAMANGE)
				{
					//float damage = localWeapon.get<float>( gEntVars.flChargedDamage );
					float damage = pBaseLocalWeapon->GetChargeDamage();
					// minium charge to be able to headshot
					if(damage < 15.0f)
					{
						return;
					}
					// respect zoom damage
					if(waitForCharge->getValue() && ZOOM_BASE_DAMAGE + damage < pTargetEntity->GetHealth())
					{
						return;
					}
				}

				// otherwise we can just shoot and kill :D
			}
			else
			{
				if(zooomOnly->getValue() && gLocalPlayerVars.wepTag.isPrimary())
				{
					return;
				}
			}
		}
		else if(gLocalPlayerVars.Class == tf_classes::TF2_Spy)
		{
			// just assume we are using the ambassador
			if(gLocalPlayerVars.activeWeapon == classId::CTFRevolver)
			{
				CTFBaseWeaponGun *tfWeap = CTFBaseWeaponGun::FromBaseEntity(pBaseLocalWeapon);

				float spread;

				try
				{
					// the one tick behind will hurt here, so its in try catch to protect
					spread = tfWeap->WeaponGetSpread();
				}
				catch(...)
				{
					spread = 0.0f;
				}

				// Log::Console("%f", spread);

				if(spread > 0)
					return;
			}
			else if(gLocalPlayerVars.activeWeapon == classId::CTFWeaponSapper)
			{
				// there is a high chance that we are not going to shoot anything with a sapper
				// this will allow users to sap sentries manually
				return;
				// TOOD add aim at buildings
			}
			else
			{
				// if we cant fire, dont aim (bullettime is a little backwards)
				if(bulletTime(pBaseLocalWeapon, true))
					return;
			}
		}
		else
		{
			// if we cant fire, dont aim (bullettime is a little backwards)
			//if(!bCanShoot(local, pUserCmd))
			//	return;
		}
	}

	//Vector hit;

	//do
	//{
	//	hit = getHitBoxVector( other, findBestHitbox( other ) );

	//	if( visible( other, hit ) == false )
	//	{
	//		targetSystem->removeTarget( target.ent );
	//		target = targetSystem->getBestTarget();
	//		if( target.ent == -1 )
	//			return;
	//		other = target.ent;
	//	}
	//	else
	//	{
	//		break;
	//	}
	//} while( true );

	//gPlayerManager.setTarget(target.ent);

	// once again use the predicted origin of our local player
	VectorAngles(target.target - ( pLocalEntity->GetViewPos() ), angles);

	ClampAngle(angles);

	// Vector oldView = pUserCmd->viewangles;

	CAimbot::silentMovementFix(pUserCmd, angles);

	pUserCmd->viewangles = angles;

	if(useSilent->getValue() != true)
		gInts->Engine->SetViewAngles(angles);

	if(autoShoot->getValue())
		pUserCmd->buttons |= IN_ATTACK; // attack when autoshoot enabled
	return;
}

bool CAimbot::isValidTarget(CBaseEntity *pBaseEntity)
{
	// global target helper already checks these, making them redundent
	//if( pBaseEntity == NULL )
	//	return false;

	//if( pBaseEntity->IsDormant() )
	//	return false;

	//if(!isPlayer(ent))
	//	return false;

	if(pBaseEntity->GetIndex() == me)
		return false;

	if(pBaseEntity->GetClientClass()->iClassID != classId::CTFPlayer)
	{
		player_info_t temp;
		if(!gInts->Engine->GetPlayerInfo(pBaseEntity->GetIndex(), &temp))
			return false;
	}
	if(pBaseEntity->IsAlive() == false)
		return false;

	if(pBaseEntity->GetTeam() == gLocalPlayerVars.team)
		return false;

	return true;
}

inline bool CAimbot::isVisibleTarget(CBaseEntity *pBaseEntity, Vector &hit)
{
	// we need to call run simulation before even getting the hitbox
	// TODO find a better way of doing this (maybe use some form of runCommand)
	if(predMode->getValue() == predictionMode::Pred_RunSim)
	{
		CUtilMove::safeRunSimulation(gInts->Prediction, pBaseEntity);
	}
	else if(predMode->getValue() == predictionMode::Pred_RunCommmand)
	{
		CUtilMove::safeRunCommand(gInts->Prediction, pBaseEntity);
	}

	int bestHitbox = findBestHitbox(pBaseEntity);

	if(bestHitbox != -1)
	{
		hit = getHitBoxVector(pBaseEntity, bestHitbox);
		return true;
	}
	else
	{
		// not visible
		return false;
	}
}

Vector CAimbot::getHitBoxVector(CBaseEntity *pBaseEntity, int hitbox)
{
	//Vector vHitbox = /*hitboxCache[{ent, hitbox}]*/{ 0,0,0 };

	//if( vHitbox == Vector( 0, 0, 0 ) )
	//{
	//	if( !isValidBuilding( ent ) )
	//	{
	//		PDWORD model = ent->GetModel();

	//		if( !model )
	//			return vHitbox;

	//		PDWORD pStudioHdr = gInts->ModelInfo->GetStudiomodel( model );

	//		if( !pStudioHdr )
	//			return vHitbox;

	//		if( !ent->SetupBones( BoneToWorld, 128, 0x100, 0 ) )
	//			return vHitbox;

	//		box = GetHitbox( hitbox, pStudioHdr );

	//		if( !box )
	//			return vHitbox;

	//		VectorTransform( box->bbmin, BoneToWorld[ box->bone ], Min );
	//		VectorTransform( box->bbmax, BoneToWorld[ box->bone ], Max );

	//		vHitbox = ( Min + Max ) / 2;

	//		// this either will predict or just return the same thing back
	//		Vector pred = predict( ent, vHitbox );

	//		//hitboxCache[{ent, hitbox}] = pred;

	//		return pred;
	//	}
	//	else
	//	{
	//		Vector temp = { 0,0,0 };
	//		ent->GetWorldSpaceCenter( temp );
	//		return temp;
	//	}
	//}
	//else
	//{
	//	// Log::Console("returning cached value of (%f,%f,%f)", vHitbox.x, vHitbox.y, vHitbox.z);
	//	return vHitbox;
	//}

	return predict(pBaseEntity, gCache.getHitboxPosition(pBaseEntity->GetIndex(), (tf_hitbox)hitbox, true));
}

inline mstudiobbox_t *CAimbot::GetHitbox(int iHitbox, DWORD *pHeader)
{
	int HitboxSetIndex = *(int *)( (DWORD)pHeader + 0xB0 );
	mstudiohitboxset_t *pSet = (mstudiohitboxset_t *)( ( (PBYTE)pHeader ) + HitboxSetIndex );

	return (mstudiobbox_t *)pSet->pHitbox(iHitbox);
}

inline bool CAimbot::isPlayer(CBaseEntity *pBaseEntity)
{
	if(pBaseEntity != NULL)
		return pBaseEntity->GetClientClass()->iClassID == classId::CTFPlayer;

	return false;
}

//int CAimbot::selectTarget()
//{
//	std::pair<int, float> target;
//
//	// this is the furthest our gun can shoot
//	// therefore it should be reasonable that all other entitys should be closer than this
//	target.second = 8192.0f;
//	target.first  = -1;
//
//	// loop through data
//	for(auto &t : stats)
//	{
//		// if we have already got the lowest distance, stop looping
//		if(target.second == 1.0f)
//			break;
//
//		float currDist = t.second;
//
//		// if priority is enabled
//		if(variables[10].vars[0].bGet())
//		{
//			vecVars &vars = variables[10].vars;
//			// scale the distances by priority
//
//			CEntity<> tar{t.first};
//
//			if(!tar.isNull())
//			{
//				if(!tar->IsDormant())
//				{
//					tf_classes Class = (tf_classes)tar.get<int>(gEntVars.iClass);
//					currDist /= vars[Class].iGet();
//				}
//			}
//		}
//
//		if(currDist < target.second)
//		{
//			target = t;
//		}
//	}
//
//	return target.first;
//}

inline bool CAimbot::checkCond(CBaseEntity *pBaseEntity)
{
	if(!ignoreCond->getValue())
		return true;

	int cond = pBaseEntity->GetCond();

	// people we shouldnt hit
	if(cond & tf_cond::TFCond_Cloaked || cond & tf_cond::TFCond_Ubercharged || cond & tf_cond::TFCond_UberchargeFading || cond & tf_cond::TFCond_CloakFlicker ||
	   cond & tf_cond::TFCond_DeadRingered || cond & tf_cond::TFCond_Bonked)
		return false;

	// TODO add netvars for cond ex and ex2

	return true;
}

// does all of the possible predictions that are needed
inline Vector CAimbot::predict(CBaseEntity *pBaseEntity, Vector v)
{
	// Vector vel{0.f,0.f,0.f};

	// clear the positions
	//positions.clear();

	CBaseEntity *localEntity = GetBaseEntity(me);

	float latency = gInts->Engine->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING) + gInts->Engine->GetNetChannelInfo()->GetLatency(FLOW_INCOMING);
	//Log::Console( "latency %f", latency );
	Vector velocity = EstimateAbsVelocity(pBaseEntity);

	if(projectilePrediction->getValue())
	{
		currPos = v;

		CBaseEntity *local = GetBaseEntity(me);
		if(local == nullptr)
			return v;
		CBaseCombatWeapon *wep = local->GetActiveWeapon();
		if(!wep)
			return v;

		F1_PredictionCache c = gCache.getPrediction(pBaseEntity->GetIndex());

		if(c.isValid)
			v += c.prediction;
		else
		{
			Vector pred = projectileHelper::PredictCorrection(wep, pBaseEntity, localEntity->GetViewPos(), projectileQuality->getValue());
			v += pred;
			gCache.storePrediction(pBaseEntity->GetIndex(), pred);
		}
	}

	if(latencyPrediction->getValue())
	{
		// scale the velocity by the latency
		//v += velocity + Vector{ latency, latency, latency };
		//v += v * GetConstantViewLag();
	}

	predPos = v;
	return v;
}

inline void CAimbot::silentMovementFix(CUserCmd *pUserCmd, Vector angles)
{
	Vector vecSilent(pUserCmd->forwardmove, pUserCmd->sidemove, pUserCmd->upmove);
	float flSpeed = sqrt(vecSilent.x * vecSilent.x + vecSilent.y * vecSilent.y);
	Vector angMove;
	VectorAngles(vecSilent, angMove);
	float flYaw = DEG2RAD(angles.y - pUserCmd->viewangles.y + angMove.y);
	pUserCmd->forwardmove = cos(flYaw) * flSpeed;
	pUserCmd->sidemove = sin(flYaw) * flSpeed;
	// pUserCmd->viewangles = angles;
}

bool CAimbot::isValidBuilding(CBaseEntity *pBaseEntity)
{
	//ClientClass *pClass = ent->GetClientClass();

	//if( pClass )
	//{
	//	switch( ent->GetClientClass()->iClassID )
	//	{
	//	case classId::CObjectTeleporter:
	//	case classId::CObjectSentrygun:
	//	case classId::CObjectDispenser:
	//		return true;
	//	default:
	//		return false;
	//	}
	//}

	return false;
}

inline int CAimbot::findBestHitbox(CBaseEntity *pBaseEntity)
{
	// if autoHitbox is not enabled then just return the desired hitbox
	if(!autoHitbox->getValue())
	{
		if(visible(pBaseEntity, (int)hitbox->getValue()))
			return (int)hitbox->getValue();
		else
			return -1;
	}

	// find the best hitbox for each class and weapon
	// by default, aim for torso
	tf_hitbox bestHitbox = tf_hitbox::spine_2;

	switch(gLocalPlayerVars.Class)
	{
		// these are the only 2 classes where we want to aim at the head (for headshotting power)
	case tf_classes::TF2_Sniper:
		// if we are using smg then aim at the chest
		if(gLocalPlayerVars.wepTag.isSecondary())
			break;
	case tf_classes::TF2_Spy:
		bestHitbox = tf_hitbox::head;
		break;
	default:
		break;
	}

	// first check if the ideal hitbox is visible
	if(visible(pBaseEntity, (int)bestHitbox))
	{
		return (int)bestHitbox;
	}
	else
	{
		// iterate through all hitboxes
		for(int i = 0; i < GetNumHitboxes(pBaseEntity); i++)
		{
			// check if the hitbox is visible
			if(visible(pBaseEntity, i))
			{
				// if this hitbox is visible, use it
				return i;
			}
		}
	}

	// if we got here then we didnt find a good hitbox.
	return -1;
}
