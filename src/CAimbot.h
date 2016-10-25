#pragma once

#include "IHack.h"
#include "Vector.h"
#include <unordered_map>
#include "CEntity.h"
#include "CTargetHelper.h"

#include "Util.h"

class CBaseEntity;

typedef unsigned long DWORD;
struct mstudiobbox_t;
typedef float matrix3x4[3][4];

class CAimbot : public IHack<CAimbot>
{
	enum predictionMode
	{
		PRED_None,
		Pred_RunSim,
		Pred_RunCommmand,
	};

	F1_ConVar<Switch> *aimbotSwitch;
	F1_ConVar<bool> *enabled;
	F1_ConVar<bool> *aimMelee;
	F1_ConVar<bool> *autoHitbox;
	F1_ConVar<Enum<tf_hitbox>> *hitbox;
	F1_ConVar<bool> *zooomOnly;
	F1_ConVar<bool> *waitForCharge;
	F1_ConVar<bool> *ignoreCond;
	F1_ConVar<bool> *ignoreFriends;
	F1_ConVar<bool> *latencyPrediction;
	F1_ConVar<bool> *autoShoot;
	F1_ConVar<bool> *aimOnClick;
	F1_ConVar<bool> *projectilePrediction;
	F1_ConVar<int> *projectileQuality;
	F1_ConVar<Enum<predictionMode>> *predMode;
	F1_ConVar<Enum<targetHelpers>> *tSystem;
	F1_ConVar<int> *fovLimit;
	F1_ConVar<bool> *useSilent;

	matrix3x4 BoneToWorld[128];
	Vector Min, Max;
	mstudiobbox_t *box;
	bool valid;
	Vector predPos, currPos;
	float speed, asymTime, time, distance;
	Vector oldAngles;
	Vector target;

	DWORD targetSystemKey;

public:
	CAimbot()
	{
		// init the prioritys
		// scout_pri.init(1, 10, 1, 1);
		// sniper_pri.init(1, 10, 1, 1);
		// soldier_pri.init(1, 10, 1, 1);
		// demo_pri.init(1, 10, 1, 1);
		// medic_pri.init(1, 10, 1, 1);
		// heavy_pri.init(1, 10, 1, 1);
		// pyro_pri.init(1, 10, 1, 1);
		// spy_pri.init(1, 10, 1, 1);
		// engi_pri.init(1, 10, 1, 1);

		aimbotSwitch = new F1_ConVar<Switch>( "Aimbot", false );
		enabled = new F1_ConVar<bool>( " - Enabled", false );
		aimMelee = new F1_ConVar<bool>( " - Aim with melee", false );
		autoHitbox = new F1_ConVar<bool>( " - Auto select hitbox", true );
		hitbox = new F1_ConVar<Enum<tf_hitbox>>( " - Hitbox", { tf_hitbox::head,
		{
			{ tf_hitbox::foot_L, "Left foot" },
			{ tf_hitbox::foot_R, "Right foot" },
			{ tf_hitbox::hand_L, "Left Hand" },
			{ tf_hitbox::hand_R, "Right hand" },
			{ tf_hitbox::head, "Head" },
			{ tf_hitbox::hip_L, "Left hip" },
			{ tf_hitbox::hip_R, "Right hip" },
			{ tf_hitbox::knee_L, "Left knee" },
			{ tf_hitbox::knee_R, "Right knee" },
			{ tf_hitbox::lowerArm_L, "Lower left arm" },
			{ tf_hitbox::lowerArm_R, "Lower right arm" },
			{ tf_hitbox::pelvis, "Pelvis" },
			{ tf_hitbox::spine_0, "Spine 0" },
			{ tf_hitbox::spine_1, "Spine 1" },
			{ tf_hitbox::spine_2, "Spine 2" },
			{ tf_hitbox::spine_3, "Spine 3" },
			{ tf_hitbox::upperArm_L, "Upper left arm" },
			{ tf_hitbox::upperArm_R, "Upper right arm" },
		}
		}, tf_hitbox::head, tf_hitbox::foot_R );

		zooomOnly = new F1_ConVar<bool>( " - Zoom only", false );
		waitForCharge = new F1_ConVar<bool>( " - Wait for charge", false );
		ignoreFriends = new F1_ConVar<bool>( " - Ignore Friends", false );
		ignoreCond = new F1_ConVar<bool>( " - Ignore Cond", false );
		latencyPrediction = new F1_ConVar<bool>( " - Latency Prediction", false );
		autoShoot = new F1_ConVar<bool>( " - Autoshoot", true );
		aimOnClick = new F1_ConVar<bool>( " - Aim on click", false );
		projectilePrediction = new F1_ConVar<bool>( " - Projectile prediction", false );
		predMode = new F1_ConVar<Enum<predictionMode>>(" - Engine prediction",
		{
			predictionMode::PRED_None,
			{
				{ PRED_None, "None" },
				{ Pred_RunSim, "Run simulation" },
				{ Pred_RunCommmand, "Run command"},
			}
			
		}, predictionMode::PRED_None, predictionMode::Pred_RunCommmand );
		tSystem = new F1_ConVar<Enum<targetHelpers>>(" - Target system",
		{
			targetHelpers::distance,
			{
				{targetHelpers::distance, "Distance"},
				{targetHelpers::fov, "FOV"}
			}
		}, targetHelpers::distance, targetHelpers::fov );
		fovLimit = new F1_ConVar<int>( " - Fov limit", 50, 1, 180, 1 );
		projectileQuality = new F1_ConVar<int>( " - - Quality", 1, 1, 10, 1 );
		useSilent = new F1_ConVar<bool>( " - Silent", true );

		valid   = false;
		predPos = currPos = {0.0f, 0.0f, 0.0f};

		//targetSystem = new CDistanceTargetSystem();

		// extrapPos = new Vector[maxExtrap];

		//Enabled = new ConVar("f1_aimbot_enabled", "0", 0);
	}

	// Inherited via IHack
	const char *name() const;
	void processCommand(CUserCmd *pUserCmd);
	bool paint();
	void menuUpdate( F1_IConVar **menuArray, int &currIndex );

private:
	// inline float getDistance(CEntity<> &ent);

	inline float getDistanceToVector(Vector v);

	inline bool visible( CBaseEntity *pBaseEntity );

	inline bool visible( CBaseEntity *pBaseEntity, int hitbox );

	inline bool visible( CBaseEntity *pBaseEntity, Vector v );

	inline void aim(CUserCmd *pUserCmd);

	inline bool isValidTarget( CBaseEntity *pBaseEntity );

	inline bool isVisibleTarget( CBaseEntity *pBaseEntity, Vector &hit );

	// this assumes the index is the target
	inline Vector getHitBoxVector( CBaseEntity *pBaseEntity, int hitbox );

	inline mstudiobbox_t *GetHitbox(int iHitbox, DWORD *pHeader);

	inline bool isPlayer( CBaseEntity *pBaseEntity );

	// inline int selectTarget();

	inline bool checkCond( CBaseEntity *pBaseEntity );

	// takes a vector and entity and scales the vector by the entitys velocity
	inline Vector predict( CBaseEntity *pBaseEntity, Vector v );

	inline void silentMovementFix(CUserCmd *pUserCmd, Vector angles);

	inline bool isValidBuilding( CBaseEntity *pBaseEntity );

	inline int findBestHitbox( CBaseEntity *pBaseEntity );
};

extern CAimbot gAimbot;
