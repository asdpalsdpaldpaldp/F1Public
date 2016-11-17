#pragma once

#include "SDK.h"

#include "CEntity.h"
#include "Util.h"
#include "Trace.h"

namespace projectileHelper
{
	inline float GetProjectileSpeed(CBaseCombatWeapon *wep);

	inline float GetProjectileGravity(CBaseCombatWeapon *wep);

	inline void PhysicsClipVelocity(const Vector &in, const Vector &normal, Vector &out, float overbounce);

	inline bool PhysicsApplyFriction(const Vector &in, Vector &out, float flSurfaceFriction, float flTickRate);

	inline void DrawDebugArrow(const Vector &vecFrom, const Vector &vecTo, const DWORD color);

	inline float GetEntityGravity(CBaseEntity *ent);

	inline unsigned int PhysicsSolidMaskForEntity(CBaseEntity *ent);

	Vector PredictCorrection(CBaseCombatWeapon *weapon, CBaseEntity *target, Vector vecFrom, int qual);

	inline void PredictPath(CBaseEntity *target);
}
