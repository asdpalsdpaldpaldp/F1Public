#pragma once

#include "SDK.h"

#include "CEntity.h"
#include "Util.h"
#include "Trace.h"

namespace projectileHelper
{

	inline float GetProjectileSpeed(CEntity<CBaseCombatWeapon> wep)
	{
		classId id = wep.castToPointer<CBaseEntity>()->GetClientClass()->iClassID;

		float BeginCharge;
		float Charge;

		//return flSpeed;
		switch(id)
		{
		case classId::CTFGrenadeLauncher:
			return 840.0f;

		//case classId::grenade:
		//	return 805.0f + (m_charge * 261.63);

		case classId::CTFRocketLauncher_DirectHit:
			return 1980.0f;

		case classId::CTFRocketLauncher:
		case classId::CTFRocketLauncher_Mortar:
			return 1100.0f;

		case classId::CTFFlareGun:
			return 1450.0f;

		case classId::CTFBat_Wood:
			return 1940.0f;

		case classId::CTFSyringeGun:
			return 990.0f;

		case classId::CTFCompoundBow:
		{
			BeginCharge = wep.get< float >( gEntVars.flChargeBeginTime );

			Charge = BeginCharge == 0.0f ? 0.0f : gInts.Globals->curtime - BeginCharge;

			if( Charge > 1.0f )
				Charge = 1.0f;

			Log::Console( "flSpeed for huntsman is %f", 800 * Charge + 1800 );

			return( 800 * Charge + 1800 );
		}

		default:
			return 1000.0f;
		}
	}

	inline float GetProjectileGravity(CEntity<CBaseCombatWeapon> wep)
	{

		classId id = wep.castToPointer<CBaseEntity>()->GetClientClass()->iClassID;

		float BeginCharge;
		float Charge;

		switch( id )
		{
		case classId::CTFCompoundBow:
			BeginCharge = wep.get< float >( gEntVars.flChargeBeginTime );

			Charge = BeginCharge == 0.0f ? 0.0f : gInts.Globals->curtime - BeginCharge;

			if( Charge > 1.0f )
				Charge = 1.0f;
			return ( ( ( 1.3 - Charge ) / 3 ) * 1000 );
			break;

		default:
			return 0.0f;
		}
	}

	__declspec(noinline) inline void PhysicsClipVelocity(const Vector &in, const Vector &normal, Vector &out, float overbounce)
	{
		float backoff = /*DotProduct(in, normal)*/ in.Dot(normal) * overbounce;

		for(int i = 0; i < 3; ++i)
		{
			float change = normal[ i ] * backoff;
			out[ i ] = in[ i ] - change;

			if(out[ i ] > -0.1f && out[ i ] < 0.1f)
				out[ i ] = 0.0f;
		}

		float adjust = /*DotProduct( out, normal )*/ out.Dot(normal);

		if ( adjust < 0.0f )
			out -= ( normal * adjust );
	}

	__declspec(noinline) inline bool PhysicsApplyFriction(const Vector &in, Vector &out, float flSurfaceFriction, float flTickRate)
	{
		static ConVar *sv_friction = gInts.Cvar->FindVar("sv_friction");
		static ConVar *sv_stopspeed = gInts.Cvar->FindVar("sv_stopspeed");

		float speed = in.Length() / flTickRate;

		if(speed < 0.1f)
			return false;

		float drop = 0.0f;

		if(flSurfaceFriction != -1.0f)
		{
			float friction = sv_friction->GetFloat() * flSurfaceFriction;
			float control = (speed < sv_stopspeed->GetFloat()) ? sv_stopspeed->GetFloat() : speed;
			drop += control * friction * flTickRate;
		}

		float newspeed = speed - drop;

		if(newspeed < 0.0f)
			newspeed = 0.0f;

		if(newspeed != speed)
		{
			newspeed /= speed;
			out = in * newspeed; // VectorScale(in, newspeed, out);
		}

		out -= in * (1.0f - newspeed);
		out *= flTickRate;
		return true;
	}

	__declspec(noinline) inline void DrawDebugArrow(const Vector &vecFrom, const Vector &vecTo, const DWORD color)
	{
		Vector angRotation;
		VectorAngles(vecTo - vecFrom, angRotation);
		Vector vecForward, vecRight, vecUp;
		AngleVectors(angRotation, &vecForward, &vecRight, &vecUp);
		gInts.DebugOverlay->AddLineOverlay(vecFrom, vecTo, RED(color), GREEN(color), BLUE(color), true, 1.0f);
		gInts.DebugOverlay->AddLineOverlay(vecFrom, vecFrom - vecRight * 10.0f, RED(color), GREEN(color), BLUE(color), true, 1.0f);
	}

	inline float GetEntityGravity(CEntity<> ent)
	{
		//static ConVar *sv_gravity = gInts.Cvar->FindVar("sv_gravity");

		moveTypes type = ent.get<moveTypes>(gEntVars.movetype);

		if( type == moveTypes::noclip || type == moveTypes::step || type == moveTypes::fly)
			return 0.0f;

		return 1.0f;
	}

	inline unsigned int PhysicsSolidMaskForEntity(CEntity<> ent)
	{
		//VIRTUAL_CALL_START(PhysicsSolidMaskForEntity, unsigned int)(IClientEntity *);
		//VIRTUAL_CALL_INIT(PhysicsSolidMaskForEntity, 128, pTarget);
		//return VIRTUAL_CALL(PhysicsSolidMaskForEntity)(pTarget);

		typedef unsigned int(__thiscall *OriginalFn)(PVOID);

		return getvfunc<OriginalFn>(ent.castToPointer<void>(), 128)(ent.castToPointer<void>());
	}

	__declspec(noinline) inline Vector PredictCorrection(CBaseCombatWeapon *pWeapon, CEntity<> target, const Vector &vecFrom, int qual)
	{
		if(!pWeapon)
			return vecFrom;

		if(target.isNull())
			return vecFrom;

		//if(IsMeleeWeapon(pWeapon))
		//	return vec3_origin;

		CEntity<CBaseCombatWeapon> weapon{ ( ( CBaseEntity * ) pWeapon )->GetIndex() };

		float flSpeed = GetProjectileSpeed(weapon);

		//float flSpeed = 1980.0f;

		if(flSpeed <= 0.0f)
			return vecFrom;

		static ConVar *sv_gravity = gInts.Cvar->FindVar("sv_gravity");
		//float flLag = /*GetConstantViewLag();*/ gInts.Engine->GetNetChannelInfo()->GetLatency(FLOW_INCOMING) + gInts.Engine->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING);
		float flLag = 0;
		bool bIsOnGround = target.get<int>(gEntVars.iFlags) & FL_ONGROUND;
		unsigned int mask = PhysicsSolidMaskForEntity(target);
		Vector vecWorldGravity = Vector(0, 0, -sv_gravity->GetFloat() * GetEntityGravity(target) * gInts.Globals->interval_per_tick * gInts.Globals->interval_per_tick);
		
		Vector vecProjGravity = Vector( 0, 0, -GetProjectileGravity( weapon ) * gInts.Globals->interval_per_tick * gInts.Globals->interval_per_tick );

		// get the current velocity
		Vector vecVelocity = EstimateAbsVelocity(target.castToPointer<CBaseEntity>());

		Vector vecProjVelocity = { flSpeed, 0, 0 };

		// get the current position
		Vector vecStepPos = target->GetAbsOrigin();

		// we can get the collideable through the netvars
		Vector vecMins = target.get<Vector>(gEntVars.collision + 0x20);
		Vector vecMaxs = target.get<Vector>(gEntVars.collision + 0x2C);

		// get velocity for a single tick
		vecVelocity *= gInts.Globals->interval_per_tick;

		vecProjVelocity *= gInts.Globals->interval_per_tick;

		Vector vecPredictedPos = vecStepPos;

		trace_t tr;
		Ray_t ray;
		//CNCTraceFilter filter(pTarget, g_NCData.m_pLP);

		CBaseFilter filter;

		// get the current arival time
		float flArrivalTime = (vecFrom - vecPredictedPos).Length() / flSpeed + flLag + gInts.Globals->interval_per_tick;
		Vector vecPredictedVel = vecVelocity;

		filter.SetIgnoreSelf( target.castToPointer<CBaseEntity>() );

		for( float flTravelTime = 0.0f; flTravelTime < flArrivalTime; flTravelTime += ( gInts.Globals->interval_per_tick  * qual) )
		{

			ray.Init( vecPredictedPos, vecPredictedPos + vecPredictedVel, vecMins, vecMaxs );
			gInts.EngineTrace->TraceRay( ray, mask, &filter, &tr );

			if( tr.startsolid )
				break;

			if( tr.fraction != 1.0f )
				PhysicsClipVelocity( vecPredictedVel, tr.plane.normal, vecPredictedVel, 1.0f );

			vecPredictedPos = tr.end;

			ray.Init( vecPredictedPos, vecPredictedPos + vecWorldGravity, vecMins, vecMaxs );
			gInts.EngineTrace->TraceRay( ray, mask, &filter, &tr );

			if( tr.fraction == 1.0f )
			{
				bIsOnGround = false;
				vecPredictedVel += vecWorldGravity;
			}
			else if( !bIsOnGround )
			{
				float surfaceFriction = 1.0f;
				gInts.PhysicsSurfaceProps->GetPhysicsProperties( tr.surface.surfaceProps, NULL, NULL, &surfaceFriction, NULL );

				if( !PhysicsApplyFriction( vecPredictedVel, vecPredictedVel, surfaceFriction, gInts.Globals->interval_per_tick ) )
					break;
			}

			flArrivalTime = ( vecFrom - vecPredictedPos ).Length() / flSpeed + flLag + gInts.Globals->interval_per_tick;
		}

		vecPredictedPos -= vecPredictedVel * flArrivalTime;

		DrawDebugArrow(vecStepPos, vecPredictedPos, COLORCODE(50, 255, 0, 255));
		return vecPredictedPos - vecStepPos;
	}

	__declspec(noinline) inline void PredictPath(CEntity<> target)
	{
		if(target.isNull())
			return;

		static ConVar *sv_gravity = gInts.Cvar->FindVar("sv_gravity");

		Vector vecVelocity = EstimateAbsVelocity(target.castToPointer<CBaseEntity>());

		bool bIsOnGround = target.get<int>(gEntVars.iFlags) & FL_ONGROUND;
		unsigned int mask = PhysicsSolidMaskForEntity(target);
		Vector vecWorldGravity = Vector(0, 0, -sv_gravity->GetFloat() * GetEntityGravity(target) * gInts.Globals->interval_per_tick * gInts.Globals->interval_per_tick);
		Vector vecStepPos = target->GetAbsOrigin();

		//auto pCollideable = target->GetCollideable();

		//if(pCollideable == nullptr)
		//	return;

		//Vector vecMins = target->GetCollideable()->OBBMins();
		//Vector vecMaxs = target->GetCollideable()->OBBMaxs();

		// we can get the collideable through the netvars
		Vector vecMins = target.get<Vector>(gEntVars.collision + 0x20);
		Vector vecMaxs = target.get<Vector>(gEntVars.collision + 0x2C);

		vecVelocity *= gInts.Globals->interval_per_tick;

		trace_t tr;
		Ray_t ray;
		CBaseFilter filter;

		filter.SetIgnoreSelf(target.castToPointer<CBaseEntity>());

		for(int i = 0; i < 100; ++i)
		{
			ray.Init(vecStepPos, vecStepPos + vecVelocity, vecMins, vecMaxs);
			gInts.EngineTrace->TraceRay(ray, mask, &filter, &tr);

			if(tr.startsolid)
				break;

			if(tr.fraction != 1.0f)
				PhysicsClipVelocity(vecVelocity, tr.plane.normal, vecVelocity, 1.0f);

			DrawDebugArrow(vecStepPos, tr.end, redGreenGradiant(i, 100));
			vecStepPos = tr.end;

			ray.Init(vecStepPos, vecStepPos + vecWorldGravity, vecMins, vecMaxs);
			gInts.EngineTrace->TraceRay(ray, mask, &filter, &tr);

			if(tr.fraction == 1.0f)
			{
				bIsOnGround = false;
				vecVelocity += vecWorldGravity;
			}
			else if(!bIsOnGround)
			{
				float surfaceFriction = 1.0f;
				gInts.PhysicsSurfaceProps->GetPhysicsProperties(tr.surface.surfaceProps, NULL, NULL, &surfaceFriction, NULL);

				if(!PhysicsApplyFriction(vecVelocity, vecVelocity, surfaceFriction, gInts.Globals->interval_per_tick))
					break;
			}
		}
	}
}