#include "SDK.h"

#include "CEntity.h"

// Reason for the critical hit result
enum iscrit_t
{
	// Server observed us firing too many crits
	ISCRIT_OBSERVED_CAP = -5,
	// Crit bucket is empty
	ISCRIT_BUCKET_EMPTY = -4,
	// CanFireCritShot() returns false
	ISCRIT_CANNOT_CRIT = -3,
	// Seed reused
	ISCRIT_REUSED_SEED = -2,
	// Just finished streaming crits, wait 1 sec
	ISCRIT_STREAM_WAIT = -1,
	// Next  crit is determined by random chance
	ISCRIT_RANDOM = 0,
	// Currently critboosted
	ISCRIT_BOOSTED = 3,
	// Currently streaming crits
	ISCRIT_STREAMING = 4,
};

// from 2007 sdk
struct WeaponData_t
{
	int m_nDamage;
	int m_nBulletsPerShot;
	float m_flRange;
	float m_flSpread;
	float m_flPunchAngle;
	float m_flTimeFireDelay;   // Time to delay between firing
	float m_flTimeIdle;		   // Time to idle after firing
	float m_flTimeIdleEmpty;   // Time to idle after firing last bullet in clip
	float m_flTimeReloadStart; // Time to start into a reload (ie. shotgun)
	float m_flTimeReload;	  // Time to reload
	bool m_bDrawCrosshair;	 // Should the weapon draw a crosshair
	int m_iProjectile;		   // The type of projectile this mode fires
	int m_iAmmoPerShot;		   // How much ammo each shot consumes
	float m_flProjectileSpeed; // Start speed for projectiles (nail, etc.); NOTE: union with something non-projectile
	float m_flSmackDelay;	  // how long after swing should damage happen for melee weapons
	bool m_bUseRapidFireCrits;

	void Init(void)
	{
		m_nDamage			= 0;
		m_nBulletsPerShot	= 0;
		m_flRange			= 0.0f;
		m_flSpread		   = 0.0f;
		m_flPunchAngle	   = 0.0f;
		m_flTimeFireDelay	= 0.0f;
		m_flTimeIdle		 = 0.0f;
		m_flTimeIdleEmpty	= 0.0f;
		m_flTimeReloadStart  = 0.0f;
		m_flTimeReload	   = 0.0f;
		m_iProjectile		= /*TF_PROJECTILE_NONE*/ 0;
		m_iAmmoPerShot	   = 0;
		m_flProjectileSpeed  = 0.0f;
		m_flSmackDelay	   = 0.0f;
		m_bUseRapidFireCrits = false;
	};
};

// Helper to calculate crits
class CTF2CritHelper
{
public:
	CTF2CritHelper();
	void Setup(int seqnr);
	bool IsCrit(iscrit_t &crit);

	bool CanFireCriticalShot();
	bool CanFireRandomCriticalShot(float flChance);
	float AttackDamage();
	void AddCritBucket(float flDamage);
	bool BlowCritBucket(float flDamage);

private:
	// State
	CEntity<> local;
	CEntity<CTFBaseWeapon> localWeapon;
	const WeaponData_t &data;
	bool bIsMelee;
	bool bStreamCrits;

	float curtime;
	int iRandomSeed;

	// Game keeps track of these stats
	struct stats_t
	{
		float flCritBucket;
		int iNumAttacks;
		int iNumCrits;
	} stats;

	// Memory layout for C_TFWeaponBase
	// place at netvar(m_bLowered) - 0x4
	struct state_t
	{
		/*0xAAC*/ bool bCurrentAttackIsCrit;
		/*0xAAD*/ bool bLowered; // NETVAR
		/*0xAB0*/ int iAltFireHint;
		/*0xAB4*/ int iReloadStartClipAmount;
		/*0xAB8*/ float flCritEndTime;
		/*0xABC*/ float flLastCritCheckTime; // NETVAR
		/*0xAC0*/ int iLastCritCheckFrame;
		/*0xAC4*/ int iCurrentSeed;
		/*0xAC8*/ float flLastCritCheckTimeNoPred;
	};

	static const DWORD stateOffset = 0x4;

	inline state_t &GetState() { return localWeapon.get<state_t>(gEntVars.bLowered - stateOffset); }

	// ConVars
	static const float tf_weapon_criticals_bucket_cap;
	static const float tf_weapon_criticals_bucket_bottom;
	static const bool tf_weapon_criticals_nopred;
};

const float CTF2CritHelper::tf_weapon_criticals_bucket_cap	= 1000.0f;
const float CTF2CritHelper::tf_weapon_criticals_bucket_bottom = -250.0f;
const bool CTF2CritHelper::tf_weapon_criticals_nopred		  = true;

CTF2CritHelper::CTF2CritHelper()
	: local(me), localWeapon(HANDLE2INDEX(local.get<CHandle>(gEntVars.hActiveWeapon))), data(localWeapon->GetWeaponData()),
	  // TODO fixme
	  bIsMelee(vars.wpn->hit == HIT_MELEE), bStreamCrits(data.m_bUseRapidFireCrits), curtime(gInts->Globals->curtime)
{
}
void CTF2CritHelper::Setup(int seqnr)
{
	// Copy the current stats
	stats = localWeapon.get<stats_t>(0x9D4);
	// Calculate the random seed
	int seedmod = (localWeapon.castToPointer<CBaseEntity>()->GetIndex() << 8) | local->GetIndex();
	if(bIsMelee)
		seedmod <<= 8;
	iRandomSeed = CRandom::MD5_PseudoRandom(seqnr) ^ seedmod;
}
bool CTF2CritHelper::IsCrit(iscrit_t &crit)
{
	// pl = pWeapon->GetOwner() and dynamic_cast to C_TFPlayer

	// pl->IsPlayer() (why?!)

	// pWeapon->CanFireCriticalShot()
	if(!CanFireCriticalShot())
	{
		crit = ISCRIT_CANNOT_CRIT;
		return false;
	}

	if(local->IsCritboosted())
	{
		crit = ISCRIT_BOOSTED;
		return true;
	}

	float flCritMult = local->GetCritMult();

	state_t &state = GetState();

	if(bIsMelee)
	{
		flCritMult = localWeapon->GetAttributeFloat(flCritMult * 0.15f, "mult_crit_chance");

		if(iRandomSeed == state.iCurrentSeed)
		{
			// Dunno predict what will happen...
			crit = ISCRIT_REUSED_SEED;
			return false;
		}
		gInts->RandomStream->SetSeed(iRandomSeed);

		// Game does some logic with local->m_iNextMeleeCrit (NETVAR)
		// int iNextMeleeCrit = local->iNextMeleeCrit();
		// state.bCurrentAttackIsCrit = iNextMeleeCrit!=0;
		// if ( iNextMeleeCrit==2 )
		//{
		//    return true;
		//}

		float flDamage = AttackDamage();
		AddCritBucket(flDamage);

		stats.iNumAttacks++;

		bool result = static_cast<float>(gInts->RandomStream->RandomInt(0, 9999)) <= (flCritMult * 10000.0f);

		if(result && !BlowCritBucket(flDamage))
		{
			crit = ISCRIT_BUCKET_EMPTY;
			return false;
		}
		else
		{
			crit = ISCRIT_RANDOM;
			return result;
		}
	}
	else
	{
		// Still streaming crits
		if(bStreamCrits && state.flCritEndTime > curtime)
		{
			crit = ISCRIT_STREAMING;
			return true;
		}

		float flDamage = AttackDamage();
		AddCritBucket(flDamage);

		//[ebp+var_1] = 0
		bool result = false;

		//[pWeapon+0xAAB] = 1

		if(bStreamCrits)
		{
			if(tf_weapon_criticals_nopred)
			{
				if(state.flLastCritCheckTimeNoPred + 1.0f > curtime)
				{
					crit = ISCRIT_STREAM_WAIT;
					return false;
				}
				// state.flLastCritCheckTimeNoPred = curtime;
			}
			else
			{
				if(state.flLastCritCheckTime + 1.0f > curtime)
				{
					crit = ISCRIT_STREAM_WAIT;
					return false;
				}
				// if ( state.flLastCritCheckTime!=curtime )
				//{
				//    state.flLastCritCheckTime = curtime;
				//}
			}

			// Modify flCritMult
			flCritMult = 1.0f / (2.0f / (flCritMult * 0.02f) - 2.0f);
			flCritMult = localWeapon->GetAttributeFloat(flCritMult, "mult_crit_chance");

			if(iRandomSeed == state.iCurrentSeed)
			{
				// Dunno predict what will happen...
				crit = ISCRIT_REUSED_SEED;
				return false;
			}
			gInts->RandomStream->SetSeed(iRandomSeed);

			result = static_cast<float>(gInts->RandomStream->RandomInt(0, 9999)) <= (flCritMult * 10000.0f);
		}
		else
		{
			// Modify flCritMult
			flCritMult = localWeapon->GetAttributeFloat(flCritMult * 0.02f, "mult_crit_chance");

			if(iRandomSeed == state.iCurrentSeed)
			{
				// Dunno predict what will happen...
				crit = ISCRIT_REUSED_SEED;
				return false;
			}
			gInts->RandomStream->SetSeed(iRandomSeed);

			result = static_cast<float>(gInts->RandomStream->RandomInt(0, 9999)) < (flCritMult * 10000.0f);
		}

		if(getmember<bool>(gpGame->pPrediction, 9))
		{
			stats.iNumAttacks++;
		}

		crit = ISCRIT_RANDOM;

		if(result)
		{
			if(CanFireRandomCriticalShot(flCritMult))
			{
				// Adjust for stream crits
				if(bStreamCrits)
				{
					flDamage = 2.0f / data.m_flTimeFireDelay * flDamage;

					if((flDamage * 3.0f) > tf_weapon_criticals_bucket_cap)
					{
						flDamage = tf_weapon_criticals_bucket_cap / 3.0f;
					}
				}

				result = BlowCritBucket(flDamage);

				if(!result)
					crit = ISCRIT_BUCKET_EMPTY;

				// if ( result )
				//{
				//    state.flCritEndTime = curtime + 2.0f;
				//}
			}
			else
			{
				crit   = ISCRIT_OBSERVED_CAP;
				result = false;
			}
		}

		return result;
	}
}
bool CTF2CritHelper::CanFireCriticalShot()
{
	return getvfunc<bool(__thiscall *)(CTFBaseWeapon *, int)>(localWeapon.castToPointer<CTFBaseWeapon>(), 0x678 / 4)(localWeapon.castToPointer<CTFBaseWeapon>(), 0);
}
bool CTF2CritHelper::CanFireRandomCriticalShot(float flChance)
{
	float flObservedCritCap = flChance + 0.1f;
	return localWeapon->flObservedCritChance() <= flObservedCritCap;
}
float CTF2CritHelper::AttackDamage()
{
	int nBulletsPerShot = bIsMelee ? 1 : static_cast<int>(localWeapon->GetAttributeFloat(static_cast<float>(data.m_nBulletsPerShot), "mult_bullets_per_shot"));
	float flDamage		 = localWeapon->GetAttributeFloat(static_cast<float>(data.m_nDamage), "mult_dmg") * static_cast<float>(nBulletsPerShot);
	return flDamage;
}
void CTF2CritHelper::AddCritBucket(float flDamage)
{
	if(stats.flCritBucket < tf_weapon_criticals_bucket_cap)
	{
		stats.flCritBucket = min(stats.flCritBucket + flDamage, tf_weapon_criticals_bucket_cap);
	}
}
bool CTF2CritHelper::BlowCritBucket(float flDamage)
{
	stats.iNumCrits++;

	// Calculate the cost of firing a critical hit
	float flCostMult;
	if(bIsMelee)
	{
		flCostMult = 0.5f;
	}
	else
	{
		float flCritRatio = static_cast<float>(stats.iNumCrits) / static_cast<float>(stats.iNumAttacks);
		flCostMult		 = (flCritRatio - 0.1f) / 0.9f;
		flCostMult		 = max(flCostMult, 0.0f);
		flCostMult		 = min(flCostMult, 1.0f);
		flCostMult		 = flCostMult * 2.0f + 1.0f;
	}

	// Cost of firing a critical hit
	float flCritCost = flDamage * 3.0f * flCostMult;

	// If not enough damage stored, deny crit
	if(flCritCost > stats.flCritBucket)
	{
		return false;
	}

	// RemoveFromCritBucket()
	stats.flCritBucket = max(stats.flCritBucket - flCritCost, tf_weapon_criticals_bucket_bottom);
	return true;
}
