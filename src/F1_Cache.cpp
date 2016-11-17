#pragma once
#include "F1_Cache.h"


F1_GlobalCache gCache;

F1_GlobalCache::F1_GlobalCache()
{}

// call at the beginning of each tick

void F1_GlobalCache::blow()
{
	for(auto &c : hbCache)
	{
		for(auto &h : c.second)
		{
			h.second.isValid = false;
		}
	}

	for(auto &c : bonesCache)
	{
		c.isValid = false;
	}

	for(auto &c : predCache)
	{
		c.isValid = false;
	}
}

bool F1_GlobalCache::isHitboxValid(int index, tf_hitbox hb)
{
	return hbCache[index][hb].isValid;
}

F1_HitboxCache F1_GlobalCache::getHitbox(int index, tf_hitbox hb)
{
	return hbCache[index][hb];
}

Vector F1_GlobalCache::getHitboxPosition(int index, tf_hitbox hb, bool recalc)
{

	auto &oldHB = getHitbox(index, hb);

	CBaseEntity *pBaseEntity = GetBaseEntity(index);

	if(pBaseEntity->IsDormant())
		return oldHB.position;

	_TRY
	{
		if(oldHB.isValid == false && recalc == true)
		{
			PDWORD model = pBaseEntity->GetModel();

			if(!model)
				return oldHB.position;

			PDWORD pStudioHdr = gInts->ModelInfo->GetStudiomodel(model);

			if(!pStudioHdr)
				return oldHB.position;

			auto box = GetHitbox((int)hb, pStudioHdr);

			if(!box)
				return oldHB.position;

			auto &bCache = bonesCache[index];

			_TRY
			{
				if(bCache.isValid == false)
				{
					if(pBaseEntity->SetupBones(bonesCache[index].BoneToWorld, 128, 0x100, gInts->Globals->curtime) == false)
					{ /*do nothing thanks*/ }

					bCache.isValid = true;
				}
			}
			_CATCH
			{
				Log::Debug("Failed SetupBones in F1_Cache::getHitboxVector()");
			}

			Vector Min, Max;
			VectorTransform(box->bbmin, bCache.BoneToWorld[box->bone], Min);
			VectorTransform(box->bbmax, bCache.BoneToWorld[box->bone], Max);

			oldHB.position = (Min + Max) / 2;

			oldHB.isValid = true;
		}
	}
	_CATCH
	{
		Log::Debug("Error from F1_Cache::getHitboxVector()");
	}

	return oldHB.position;
}

F1_PredictionCache F1_GlobalCache::getPrediction(int index)
{
	return predCache[index];
}

void F1_GlobalCache::storePrediction(int index, Vector pred)
{
	F1_PredictionCache t;
	t.isValid = true;
	t.prediction = pred;
	predCache[index] = t;
}
