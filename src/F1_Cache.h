#pragma once

#include <unordered_map>
#include <vector>

#include "SDK.h"

#include "Util.h"

#include "CTargetHelper.h"


struct F1_HitboxCache
{
	Vector position;
	bool isValid = false;
};

struct F1_BonesCache
{
	matrix3x4 BoneToWorld[ 128 ];
	bool isValid = false;
};


class F1_GlobalCache
{

	std::unordered_map<int, std::unordered_map<tf_hitbox, F1_HitboxCache>> hbCache;

	std::unordered_map<int, F1_BonesCache> bonesCache;
	
public:

	F1_GlobalCache()
	{}

	// call at the beginning of each tick
	void blow()
	{
		for(auto &c : hbCache )
		{
			for(auto &h : c.second )
			{
				h.second.isValid = false;
			}
		}

		for(auto &c : bonesCache )
		{
			c.second.isValid = false;
		}
	}

	bool isValid(int index, tf_hitbox hb)
	{
		return hbCache[ index ][ hb ].isValid;
	}

	Vector getHitboxPosition(int index, tf_hitbox hb, bool recalc = false)
	{

		auto &oldHB = hbCache[ index ][ hb ];

		CBaseEntity *pBaseEntity = gInts.EntList->GetClientEntity( index );

		if( pBaseEntity->IsDormant() )
			return oldHB.position;

		_TRY
		{
			if( oldHB.isValid == false && recalc == true)
			{
				PDWORD model = pBaseEntity->GetModel();

				if( !model )
					return oldHB.position;

				PDWORD pStudioHdr = gInts.ModelInfo->GetStudiomodel( model );

				if( !pStudioHdr )
					return oldHB.position;

				auto box = GetHitbox( (int)hb, pStudioHdr );

				if( !box )
					return oldHB.position;

				auto &bCache = bonesCache[ index ];

				if( bonesCache[ index ].isValid == false )
				{
					if( pBaseEntity->SetupBones( bonesCache[ index ].BoneToWorld, 128, 0x100, gInts.Globals->curtime ) == false )
						throw;

					bonesCache[ index ].isValid = true;
				}

				Vector Min, Max;
				VectorTransform( box->bbmin, bCache.BoneToWorld[ box->bone ], Min );
				VectorTransform( box->bbmax, bCache.BoneToWorld[ box->bone ], Max );

				oldHB.position = ( Min + Max ) / 2;

				oldHB.isValid = true;
			}
		}
		_CATCH
		{
			Log::Debug( "Error from F1_Cache::getHitboxVector()" );
		}

		return oldHB.position;
	}

	F1_HitboxCache getHitbox(int index, tf_hitbox hb)
	{
		return hbCache[ index ][ hb ];
	}
};

extern F1_GlobalCache gCache;