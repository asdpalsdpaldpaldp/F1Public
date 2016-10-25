#pragma once

#include <unordered_map>
#include <vector>

#include "SDK.h"

#include "Util.h"

#include "CTargetHelper.h"

struct F1_BaseCache
{
	bool isValid = false;
};

struct F1_HitboxCache : public F1_BaseCache
{
	Vector position;
};

struct F1_BonesCache : public F1_BaseCache
{
	matrix3x4 BoneToWorld[ 128 ];
};

struct F1_PredictionCache : public F1_BaseCache
{
	Vector prediction;
};


class F1_GlobalCache
{

	std::unordered_map<int, std::unordered_map<tf_hitbox, F1_HitboxCache>> hbCache;

	//std::unordered_map<int, F1_BonesCache> bonesCache;

	std::vector<F1_BonesCache> bonesCache = std::vector<F1_BonesCache>(33, {});

	std::vector<F1_PredictionCache> predCache = std::vector<F1_PredictionCache>(33, {});

	//std::unordered_map<int, F1_PredictionCache> predCache;
	
public:

	F1_GlobalCache();

	// call at the beginning of each tick
	void blow();

	bool isHitboxValid(int index, tf_hitbox hb);

	inline F1_HitboxCache getHitbox(int index, tf_hitbox hb);

	Vector getHitboxPosition(int index, tf_hitbox hb, bool recalc = false);

	F1_PredictionCache getPrediction(int index);

	void storePrediction(int index, Vector pred);
};

extern F1_GlobalCache gCache;
