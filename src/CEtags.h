#pragma once
#include <vector>

#include "ClassId.h"

class CBaseEntity;

enum class EntTag : int
{
	NO_TAGS = 0,
	PLAYER = (1 << 0),
	BUILDING = (1 << 1),
	SENTRY = (1 << 2),
	DISPENSER = (1 << 3),
	TELEPORTER = (1 << 4),
	WEAPON = (1 << 5),
	PRIMARY = (1 << 6),
	SECONDARY = (1 << 7),
	MELEE = (1 << 8),
	PDA = (1 << 9),
	PROJECTILE = (1 << 10),
};

// and operator
inline EntTag operator&(EntTag local, EntTag other)
{
	return static_cast<EntTag>(static_cast<int>(local) & static_cast<int>(other));
}
// or operator
inline EntTag operator|(EntTag local, EntTag other)
{
	return static_cast<EntTag>(static_cast<int>(local) | static_cast<int>(other));
}
// and equals
inline EntTag operator&=(EntTag &local, EntTag other)
{
	int &loc = (int &)local;

	return static_cast<EntTag>(loc &= static_cast<int>(other));
}
// or equals operator
inline EntTag operator|=(EntTag &local, EntTag other)
{
	int &loc = (int &)local;

	return static_cast<EntTag>(loc |= static_cast<int>(other));
}

class CEntTag
{
	EntTag tags;

public:
	CEntTag(CBaseEntity *ent);
	CEntTag() : tags(EntTag::NO_TAGS) {};

	// get the current tags for manual looking
	EntTag getTags();

	// helper functions
	bool isPlayer();
	bool isBuilding();
	bool isSentry();
	bool isDispenser();
	bool isTele();
	bool isWeap();
	bool isPrimary();
	bool isSecondary();
	bool isMelee();
	bool isPda();
	bool isProjectile();
};
