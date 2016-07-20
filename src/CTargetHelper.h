#pragma once

#include <unordered_map>

#include "baseHeaders.h"

#include "CEntity.h"

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

		float mag = sqrtf(pow(aim.x, 2) + pow(aim.y, 2) + pow(aim.z, 2));
		float u_dot_v = aim.Dot(ang);

		return RAD2DEG(acos(u_dot_v / (pow(mag, 2))));
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
		return std::hash<int>()(c.ent);
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

