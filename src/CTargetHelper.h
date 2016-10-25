#pragma once

#include <unordered_map>

#include "baseHeaders.h"

#include "CEntity.h"

#include "CPlayerManager.h"

namespace __CTargetHelper
{
	// makes me ill
	inline Vector CalcAngle(Vector PlayerPos, Vector EnemyPos)
	{
		Vector AimAngles;
		Vector delta = PlayerPos - EnemyPos;
		float hyp = sqrtf((delta.x * delta.x) + (delta.y * delta.y)); //SUPER SECRET IMPROVEMENT CODE NAME DONUT STEEL
		AimAngles.x = atanf(delta.z / hyp) * RADPI;
		AimAngles.y = atanf(delta.y / delta.x) * RADPI;
		AimAngles.z = 0.0f;
		if(delta.x >= 0.0)
			AimAngles.y += 180.0f;

		return AimAngles;
	}

	inline void MakeVector(Vector angle, Vector& vector)
	{
		float pitch = float(angle[0] * PI / 180);
		float yaw = float(angle[1] * PI / 180);
		float tmp = float(cos(pitch));
		vector[0] = float(-tmp * -cos(yaw));
		vector[1] = float(sin(yaw)*tmp);
		vector[2] = float(-sin(pitch));
	}

	inline float GetFov(Vector angle, Vector src, Vector dst)
	{
		Vector ang, aim;
		ang = CalcAngle(src, dst);

		MakeVector(angle, aim);
		MakeVector(ang, ang);

		float mag = aim.Length();
		float u_dot_v = aim.Dot(ang);

		return RAD2DEG(acos(u_dot_v / (pow(mag, 2))));
	}

	inline float GetFovFromLocalPlayer( Vector dst )
	{
		CEntity<> local{ me };
		return GetFov( local->GetAbsAngles(), local->GetAbsOrigin() + gLocalPlayerVars.viewOffset, dst );
	}

	inline float getDistanceToVector(Vector v)
	{
		CBaseEntity *pLocal = gInts.EntList->GetClientEntity(me);

		if(!pLocal)
			return 8192.0f;

		return pLocal->GetAbsOrigin().DistToSqr( v );
	}
}

struct CTarget
{
	int ent;
	Vector target;

	bool operator==(const CTarget other) const
	{
		return ent == other.ent;
	}
};

template<>
struct std::hash<CTarget>
{
	size_t operator()(const CTarget &c) const
	{
		// we dont need to hash this, each index is unique anyway
		return c.ent;
	}
};

enum class targetHelpers
{
	distance,
	fov,
};

// allows the user to get the best target of that system without needing to know the impl
class IBaseTargetSystem
{
public:
	virtual CTarget getBestTarget() = 0;

	virtual void removeTarget(int index) = 0;

	virtual void addTarget(CTarget index) = 0;

	virtual CTarget getLastBestTarget() = 0;

	virtual float getBestStat() = 0;

	virtual targetHelpers getType() = 0;
};

// target system based on distance
// gets the closest target
class CDistanceTargetSystem : public IBaseTargetSystem
{
	using target = std::pair<CTarget, float>;

	std::unordered_map<CTarget, float> targets;

	CTarget lastBestTarget;

public:
	CDistanceTargetSystem() { targets[{-1, {0,0,0}}] = FLT_MAX; }

	CTarget getBestTarget() override
	{
		target bestTarg({-1, {0,0,0}}, FLT_MAX);

		for(auto &targ : targets)
		{
			if(targ.second < bestTarg.second)
			{
				bestTarg		 = targ;
				lastBestTarget = bestTarg.first;
			}
		}

		return bestTarg.first;
	}

	// doesnt recalculate best target
	float getBestStat() override { return targets[lastBestTarget]; }

	void addTarget(CTarget targ) override
	{
		// add it to tge list
		targets[targ] = __CTargetHelper::getDistanceToVector(targ.target);
	}

	CTarget getLastBestTarget() override
	{
		return lastBestTarget;
	}

	void removeTarget(int index) override { targets.erase({index, {0,0,0}}); }

	targetHelpers getType() override { return targetHelpers::distance; };
};

class CFovTargetSystem : public IBaseTargetSystem
{

	using target = std::pair<CTarget, float>;

	std::unordered_map<CTarget, float> targets;

	CTarget lastBestTarget;

public:
	CFovTargetSystem() { targets[{-1, {0,0,0}}] = 8192.0f; };

	CTarget getBestTarget() override
	{
		target bestTarg({-1,{0,0,0}}, 8192.f);

		for(auto &targ : targets)
		{
			if(targ.second < bestTarg.second)
			{
				bestTarg = targ;
				lastBestTarget = bestTarg.first;
			}
		}

		return bestTarg.first;
	}

	float getBestStat() override { return targets[lastBestTarget]; }

	void addTarget(CTarget index) override
	{
		CEntity<> local{me};

		float fov = __CTargetHelper::GetFov(local->GetAbsAngles(), local->GetAbsOrigin() + gLocalPlayerVars.viewOffset, index.target);
		//Log::Console("Adding Target %d with fov %f", index, fov);
		targets[index] = fov;
	}

	CTarget getLastBestTarget() override
	{
		return lastBestTarget;
	}

	void removeTarget(int index) override { targets.erase({index, {0,0,0}}); }

	targetHelpers getType() override { return targetHelpers::fov; };
};

inline IBaseTargetSystem *switchTargetSystem(IBaseTargetSystem *oldPtr, targetHelpers newType)
{
	if(oldPtr)
		delete oldPtr;

	switch(newType)
	{
	case targetHelpers::distance:
		return new CDistanceTargetSystem();
	case targetHelpers::fov:
		return new CFovTargetSystem();
	default:
		Log::Debug("INVALID TARGET SYSTEM ID: %d", newType);
		return oldPtr;
		break;
	}
}

class CGlobalTargetHelper
{
	CTarget bestRageTarg;
	CTarget bestTarg;

	CTarget targ;
public:

	// return value is whether the entity is valid (e.g. for player cond, alive, team)
	// int is the index
	// this should not check whether the target is visible (e.g. position checks)
	using isValidTargetFn = std::function<bool( int )>;

	// return value is whether the entity is visible
	// int is the index
	// place the target vector into the Vector &
	// this should only do position checks
	using isVisibleTargetFn = std::function<bool( int, Vector & )>;

	// compares two targets too each other
	// the first param is the current best target
	// the second param is the new potential best target
	// return true to show that the new one is better
	// or false to show that the old one is still best
	using compareTargetFn = std::function<bool( const CTarget &, const CTarget & )>;

	CGlobalTargetHelper()
	{
		isValidTarget = defaultIsValidTargetFn;
		isVisibleTarget = defaultIsVisibleTargetFn;
		compareTarget = defaultCompareTargetFn;
	}

	// evalulates targets based on system used, rage targets, friends
	// call once per tick
	void think()
	{
		bool oneTimeRage = false;
		bool oneTimeNormal = false;

		bool hasRageTarget = false;
		bool hasTarget = false;

		for( int i = 1; i < gInts.Engine->GetMaxClients(); i++ )
		{
			CBaseEntity *pBaseEntity = gInts.EntList->GetClientEntity( i );

			if( pBaseEntity == NULL )
			{
				continue;
			}

			if( pBaseEntity->IsDormant() )
			{
				continue;
			}

			if( !isValidTarget( i ) )
			{
				continue;
			}

			Vector t;

			if( !isVisibleTarget( i, t ) )
			{
				continue;
			}

			if( gPlayerManager.getModeForPlayer( i ) == CPlayerManager::playerMode::Rage )
			{
				CTarget newTarg = {i,t};
				if(!oneTimeRage)
				{
					bestRageTarg = newTarg;
					oneTimeRage = true;
					hasRageTarget = hasTarget = true;
				}
				else
				{
					if(compareTarget(bestRageTarg, newTarg))
					{
						bestRageTarg = newTarg;
					}
				}
			}
			else if( gPlayerManager.getModeForPlayer( i ) == CPlayerManager::playerMode::Normal )
			{
				CTarget newTarg = {i,t};
				if(!oneTimeNormal)
				{
					bestTarg = newTarg;
					oneTimeNormal = true;
					hasTarget = true;
				}
				else
				{
					if(compareTarget(bestTarg, newTarg))
					{
						bestTarg = newTarg;
					}
				}
			}
		}

		if(hasTarget == true)
		{
			// if we have a rage target, then we want to hit them
			if(hasRageTarget == true)
				targ = bestRageTarg;
			else
				targ = bestTarg;

			gPlayerManager.setTarget(targ.ent);
		}
		else
		{
			// set invalid ents
			gPlayerManager.setTarget(-1);
			targ.ent = -1;
		}


	}

	CTarget getBestTarget()
	{
		return targ;
	}

	void setValidTargetFn( DWORD k, isValidTargetFn fn )
	{
		if( k != key )
			return;
		if( fn == nullptr )
		{
			isValidTarget = defaultIsValidTargetFn;
		}
		isValidTarget = fn;
	}

	void setVisibleTargetFn( DWORD k, isVisibleTargetFn fn )
	{
		if( k != key )
			return;
		if( fn == nullptr )
		{
			isVisibleTarget = defaultIsVisibleTargetFn;
		}
		isVisibleTarget = fn;
	}

	void setCompareTarget( DWORD k, compareTargetFn fn )
	{
		if( k != key )
			return;
		if( fn == nullptr )
		{
			compareTarget = defaultCompareTargetFn;
		}
		compareTarget = fn;
	}

	void releaseOwner( DWORD k )
	{
		if( key == k )
		{
			owned = false;
			key += 1;
		}
	}

	bool tryBecomeOwner( DWORD &k )
	{
		if( owned )
			return false;
		else
			k = key;

		return true;
	}

private:
	isValidTargetFn isValidTarget;
	isValidTargetFn defaultIsValidTargetFn = []( int ) -> bool { return false; };

	isVisibleTargetFn isVisibleTarget;
	isVisibleTargetFn defaultIsVisibleTargetFn = []( int, Vector & ) -> bool { return false; };

	compareTargetFn compareTarget;
	compareTargetFn defaultCompareTargetFn = []( const CTarget &, const CTarget & ) -> bool { return false; };

	// this is here to prevent different modules from overriding the valid/visible/compare functions
	// only 1 module should needs to get targets at a time
	DWORD key;
	bool owned = false;

};

extern CGlobalTargetHelper gTargetHelper;

