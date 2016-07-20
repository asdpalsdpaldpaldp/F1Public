#pragma once

#include "IHack.h"
#include "F1_Gui.h"
#include "CDrawManager.h"
#include "CEntity.h"

#include <mutex>

class CPlayerManager : public IHack, public IGameEventListener2
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
		F1_ConVar<Enum<playerMode>> *mode = NULL;
		bool isTarget = false;
		bool isValid = false;
		int uid = -1;
	};
	
	CPlayerManager();

	void init() override;

	// returns -1 or 0xFFFFFFFF on no custom color (use team color)
	DWORD getColorForPlayer( int index );

	playerMode getModeForPlayer( int index );

	void menuUpdate( F1_IConVar **menuArray, int &currIndex ) override;

	bool paint() override;

	void setTarget( int index );

	void FireGameEvent( IGameEvent *event ) override;

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

	std::mutex playerMutex;
};

extern CPlayerManager gPlayerManager;