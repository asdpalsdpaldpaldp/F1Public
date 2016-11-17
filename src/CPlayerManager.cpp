#include "CPlayerManager.h"

CPlayerManager gPlayerManager;

CPlayerManager::CPlayerManager()
{}

void CPlayerManager::init()
{
	playerModeSwitch = new F1_ConVar<Switch>("Player List", false);

	gInts->EventManager->AddListener(this, "player_disconnect", false);
	gInts->EventManager->AddListener(this, "player_connect_client", true);
	gInts->EventManager->AddListener(this, "teamplay_round_start", false);
}

DWORD CPlayerManager::getColorForPlayer(int index)
{
	_TRY
	{
		// this is SLOW! TODO: we need to bake this in somewhere
		auto info = gInts->Engine->GetPlayerInfo(index);

		if(playerArray[info.userID].isTarget == true)
		{
			return COLORCODE(170, 0, 255, 255);
		}
		else
		{
			switch(playerArray[info.userID].mode.getValue())
			{
			case playerMode::Normal:
				return 0xFFFFFFFF;
				break;
			case playerMode::Friend:
				return 0x00FF32FF;
				break;
			case playerMode::Rage:
				return 0xFF0000FF;
				break;
			}
		}
	}
		_CATCH
	{
	}

	// return white by default (-1)
	return 0xFFFFFFFF;
}

CPlayerManager::playerMode CPlayerManager::getModeForPlayer(int index)
{
	_TRY
	{
		player_info_t info;
		if(gInts->Engine->GetPlayerInfo(index, &info))
		{
			return playerArray[info.userID].mode.getValue();
		}
	}
		_CATCH
	{
	}

	return playerMode::Normal;
}

void CPlayerManager::menuUpdate(F1_IConVar **menuArray, int &currIndex)
{
	menuArray[currIndex++] = playerModeSwitch;

	if(playerModeSwitch->getValue())
	{
		for(auto &p : playerArray)
		{
			if(p.second.isValid == true)
				menuArray[currIndex++] = &p.second.mode;
		}
	}
}

bool CPlayerManager::paint()
{
	if(gInts->Engine->IsConnected())
	{
		if(!isInGame)
		{
			registerAllPlayers();
			isInGame = true;
		}
	}
	else
	{
		isInGame = false;
	}

	return false;
}

void CPlayerManager::setTarget(int index)
{
	if(index != -1 && index > 1)
	{
		player_info_t info;

		if(gInts->Engine->GetPlayerInfo(index, &info))
		{
			if(lastTargetIndex != -1)
			{
				playerArray[lastTargetIndex].isTarget = false;
			}

			playerArray[info.userID].isTarget = true;

			lastTargetIndex = info.userID;
		}
	}
	else
	{
		playerArray[lastTargetIndex].isTarget = false;
		lastTargetIndex = -1;
	}
}

std::vector<CPlayerManager::playerState *> CPlayerManager::getPlayersWithMode(playerMode mode)
{
	std::vector<playerState *> v;

	for(auto &p : playerArray)
	{
		if(p.second.mode.getValue() == mode)
			v.push_back(&p.second);
	}

	return v;
}

void CPlayerManager::FireGameEvent(IGameEvent *event)
{
	_TRY
	{
		if(gLocalPlayerVars.info.userID == 0)
			return;

		if(!strcmp(event->GetName(), "player_disconnect"))
		{
			deregisterPlayerEvent(event);
		}
		else if(!strcmp(event->GetName(), "player_connect_client"))
		{
			registerPlayerEvent(event);
		}
		else if(!strcmp(event->GetName(), "teamplay_round_start"))
		{
			Log::Console("Round Start - Reset Player list");
			registerAllPlayers();
		}
	}
		_CATCH
	{
	}
}

void CPlayerManager::registerPlayerEvent(IGameEvent *event)
{
	Log::Console("Player Register");

	auto name = event->GetString("name", "<null>");
	auto uid = event->GetInt("userid");

	char newName[35] = " - ";

	strcat_s(newName, name);
	playerArray[uid].mode = F1_ConVar<Enum<playerMode>>(newName, playerStateEnum, playerMode::Normal, playerMode::Rage);

	playerArray[uid].isValid = true;
}

void CPlayerManager::registerPlayerEntity(CBaseEntity *entity)
{
	Log::Console("Player Register");

	player_info_t playerInfo = gInts->Engine->GetPlayerInfo(entity->GetIndex());

	char newName[35] = " - ";

	strcat_s(newName, playerInfo.name);
	playerArray[playerInfo.userID].mode = F1_ConVar<Enum<playerMode>>(newName, playerStateEnum, playerMode::Normal, playerMode::Rage);

	playerArray[playerInfo.userID].isValid = true;
}

void CPlayerManager::deregisterPlayerEvent(IGameEvent *event)
{
	Log::Console("Player Deregister");

	auto uid = event->GetInt("userid");

	//playerArray[ uid ].mode;
	playerArray[uid].isValid = false;
	playerArray[uid].uid = -1;
}

void CPlayerManager::deregisterAllPlayers()
{
	playerArray.clear();
}

void CPlayerManager::registerAllPlayers()
{
	deregisterAllPlayers();
	for(int i = 0; i < gInts->EntList->GetHighestEntityIndex(); i++)
	{
		auto *player = GetBaseEntity(i);

		if(player == NULL)
			continue;
		if(player->GetIndex() == me)
			continue;
		if(player->GetClientClass()->iClassID == classId::CTFPlayer)
		{
			registerPlayerEntity(player);
		}
	}
}
