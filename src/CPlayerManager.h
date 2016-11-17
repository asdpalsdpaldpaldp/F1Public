#pragma once

#include "IHack.h"
#include "F1_Gui.h"
#include "CDrawManager.h"
#include "CEntity.h"

#include <mutex>

class CPlayerManager : public IHack<CPlayerManager>, public IGameEventListener2
{
public:

	enum class playerMode
	{
		Normal,
		Friend,
		Rage,
	};

	struct playerState
	{
		//playerMode mode;
		F1_ConVar<Enum<playerMode>> mode{ "<default>",{
			playerMode::Normal,
			{
				{ playerMode::Normal, "Normal" },
				{ playerMode::Friend, "Friend" },
				{ playerMode::Rage, "Rage" },
			}
		} , playerMode::Normal, playerMode::Rage };
		bool isTarget = false;
		bool isValid = false;
		int uid = -1;
	};

	CPlayerManager();

	void init();

	// returns -1 or 0xFFFFFFFF on no custom color (use team color)
	DWORD getColorForPlayer(int index);

	playerMode getModeForPlayer(int index);

	void menuUpdate(F1_IConVar **menuArray, int &currIndex);

	bool paint();

	void setTarget(int index);

	std::vector<playerState *> getPlayersWithMode(playerMode mode);

	void FireGameEvent(IGameEvent *event) override;

	void registerPlayerEvent(IGameEvent *event);
	void registerPlayerEntity(CBaseEntity *entity);
	void deregisterPlayerEvent(IGameEvent *event);
	void deregisterAllPlayers();
	void registerAllPlayers();

private:

	std::unordered_map<int, playerState> playerArray;

	int lastTargetIndex = -1;

	F1_ConVar<Switch> *playerModeSwitch;

	Enum<playerMode> playerStateEnum =
	{
		playerMode::Normal,
		{
			{ playerMode::Normal, "Normal" },
			{ playerMode::Friend, "Friend" },
			{ playerMode::Rage, "Rage" },
		}
	};

	bool isInGame = false;
};

extern CPlayerManager gPlayerManager;
