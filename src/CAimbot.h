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


struct EntityHitbox_t
{
	CEntity<> e;
	int hitbox;

	bool operator==(const EntityHitbox_t &other) const
	{
		return e.index() == other.e.index() && hitbox == other.hitbox;
	}
};

template<>
struct std::hash<EntityHitbox_t>
{
	size_t operator()(const EntityHitbox_t &other) const
	{
		size_t hash = std::hash<int>()(other.e.index());
		hash_combine<int>(hash, other.hitbox);
		return hash;
	}
};

class CAimbot : public IHack
{

	//bool enabled;
	//int hitbox;
	//bool zoomOnly;
	//bool respectChargedDamage;
	//float zoomLowestDamage;
	//bool ignoreCond;
	//bool ignoreFriends;
	//bool velocityPrediction;
	//bool aimOnClick;
	//bool silentFix;
	//bool projectilePrediction;
	//bool noSilentAim;
	//bool runSimulation;
	//int tSystem;
	//int fovLimit;
	//bool classPriorityEnabled;



	//VAR( bool, enabled, "Enabled" );
	//VAR( bool, aimMelee, "Aim for melee?" );
	//VAR( bool, autoHitbox, "Auto hitbox" );
	//VAR( int, hitbox, "Hitbox number", 0, 17, 0, 1 );
	//VAR( bool, zoomOnly, "Zoomed only" );
	//VAR( bool, respectChargedDamage, "Charged damage" );
	//VAR( float, zoomLowestDamage, "Lowest damage", 0.0f, 100.0f, 15.0f, 1.0f );
	//VAR( bool, ignoreCond, "Ignore cond players" );
	//VAR( bool, ignoreFriends, "Ignore friends" );
	//VAR( bool, velocityPrediction, "Velocity Prediction" );
	//VAR( bool, click, "Aim on click" );
	//VAR( bool, silentFix, "Silent movement fix" );
	//VAR( bool, projectilePrediction, "Projectile prediction" );
	//VAR( bool, noSilentAim, "No silent" );
	//VAR( bool, runSimulation, "Other pred (glitchy)" );
	//VAR( int, tSystem, "Target system", 0, 1, 0, 1 );
	//VAR( int, fovLimit, "Max FOV", 0, 180, 60, 5 );

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

	// vars for priority
	//var priority_list_switch{"Priority", &priority_list};

	//int classPriorities[9];

	//var enable_pri{"Enable priority", &classPriorityEnabled};
	//var scout_pri{"Scout", &classPriorities[0], 1, 10, 1, 1};
	//var sniper_pri{"Sniper", &classPriorities[1], 1, 10, 1, 1};
	//var soldier_pri{"Soldier", &classPriorities[2], 1, 10, 1, 1};
	//var demo_pri{"Demoman", &classPriorities[3], 1, 10, 1, 1};
	//var medic_pri{"Medic", &classPriorities[4], 1, 10, 1, 1};
	//var heavy_pri{"Heavy", &classPriorities[5], 1, 10, 1, 1};
	//var pyro_pri{"Pyro", &classPriorities[6], 1, 10, 1, 1};
	//var spy_pri{"Spy", &classPriorities[7], 1, 10, 1, 1};
	//var engi_pri{"Engi", &classPriorities[8], 1, 10, 1, 1};

	matrix3x4 BoneToWorld[128];
	Vector Min, Max;
	mstudiobbox_t *box;
	bool valid;
	Vector predPos, currPos;
	float speed, asymTime, time, distance;
	Vector oldAngles;
	Vector target;

	// std::vector<Point> positions;

	// maps entitys to their distance
	// if an entity is on the list it has to both be valid and visible

	// std::unordered_map<int, float> stats;

	//CDistanceTargetSystem targetSystem;

	IBaseTargetSystem *targetSystem = nullptr;

	ConVar *Enabled;


	// TODO this might not be the best soloution
	std::unordered_map<EntityHitbox_t, Vector> hitboxCache;

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

		targetSystem = new CDistanceTargetSystem();

		// extrapPos = new Vector[maxExtrap];

		//Enabled = new ConVar("f1_aimbot_enabled", "0", 0);
	}

	// Inherited via IHack
	virtual const char *name() const override;
	virtual void processCommand(CUserCmd *pUserCmd) override;
	virtual bool paint() override;
	virtual bool processEntity(int index) override;
	void menuUpdate( F1_IConVar **menuArray, int &currIndex ) override;

private:
	// inline float getDistance(CEntity<> &ent);

	inline float getDistanceToVector(Vector v);

	inline bool visible(CEntity<> &ent);

	inline bool visible(CEntity<> &ent, int hitbox);

	inline bool visible(CEntity<> &ent, Vector v);

	inline void aim(CUserCmd *pUserCmd);

	inline bool isValidTarget(CEntity<> &ent);

	// this assumes the index is the target
	inline Vector getHitBoxVector(CEntity<> &ent, int hitbox);

	inline mstudiobbox_t *GetHitbox(int iHitbox, DWORD *pHeader);

	inline bool isPlayer(CEntity<> &ent);

	// inline int selectTarget();

	inline bool checkCond(CEntity<> &ent);

	// takes a vector and entity and scales the vector by the entitys velocity
	inline Vector predict(CEntity<> &ent, Vector v);

	inline void silentMovementFix(CUserCmd *pUserCmd, Vector angles);

	inline bool isValidBuilding(CEntity<> &ent);

	inline int findBestHitbox(CEntity<> &ent);
};
