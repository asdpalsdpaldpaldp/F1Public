#include "CNokick.h"

#include "SDK.h"
#include "CEntity.h"

#include <ctime>

CNokick gNokick;

// bewarned, this is all FUCKING USELESS NOW. YAY

typedef void(__thiscall *EmitSoundFn)(PVOID pSound, IRecipientFilter &filter, int iEntIndex, int iChannel, const char *pSample, float flVolume,
									  float flAttenuation, int iFlags, int iPitch /*= PITCH_NORM*/, int iSpecialDSP, const Vector *pOrigin, const Vector *pDirection,
									  void *pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity);

void __stdcall Hooked_PlaySound(const char *soundName) { return CNokick::getInst()->playSound(soundName); }

void __fastcall Hooked_EmitSound(PVOID pSound, int edx, IRecipientFilter &filter, int iEntIndex, int iChannel, const char *pSample, float flVolume,
								 float flAttenuation, int iFlags, int iPitch /*= PITCH_NORM*/, int iSpecialDSP, const Vector *pOrigin, const Vector *pDirection,
								 void *pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity)
{
	CNokick::getInst()->emitSound(pSample);

	// call the original function
	// not calling it causes wierd effects
	auto &hook = VMTManager::GetHook(gInts->SoundEngine);
	hook.GetMethod<EmitSoundFn>(gOffsets.emitSound)(pSound, filter, iEntIndex, iChannel, pSample, flVolume,
													flAttenuation, iFlags, iPitch, iSpecialDSP, pOrigin, pDirection, pUtlVecOrigins,
													bUpdatePositions, soundtime, speakerentity);
}

void __fastcall Hooked_UserDispatchMessage(PVOID CHLClient, int edx, int type, bf_read &data)
{
	CNokick::getInst()->userMessage(type, data);

	auto &hook = VMTManager::GetHook(CHLClient);
	hook.GetMethod<void(__thiscall *)(PVOID, int, bf_read &)>(gOffsets.dispatchUserMessageOffset)(CHLClient, type, data);
}

CNokick::CNokick()
{
	voteKick = false;
	yesVote = noVote = teamPlayers = 0;
}

void CNokick::init()
{
	//playSoundHook = new VMTBaseManager();
	//playSoundHook->Init(gInts->Surface);
	//playSoundHook->HookMethod(&Hooked_PlaySound, gOffsets.playSound);
	//playSoundHook->Rehook();

	//emitSoundHook = new VMTBaseManager();
	//emitSoundHook->Init(gInts->SoundEngine);
	//emitSoundHook->HookMethod(&Hooked_EmitSound, gOffsets.emitSound);
	//emitSoundHook->Rehook();

	//gHookManager.hookMethod(gInts->Surface, gOffsets.playSound, &Hooked_PlaySound);
	gHookManager.hookMethod(gInts->Surface, gOffsets.playSound, &Hooked_PlaySound);
	gHookManager.hookMethod(gInts->SoundEngine, gOffsets.emitSound, &Hooked_EmitSound);

	gHookManager.hookMethod(gInts->Client, gOffsets.dispatchUserMessageOffset, &Hooked_UserDispatchMessage);

	return;
}

DWORD WINAPI disconnectRetry(LPVOID arg)
{
	gInts->Engine->ClientCmd_Unrestricted("disconnect");
	gInts->Cvar->ConsolePrintf("====================================\nRECONNECTING IN 10 SECONDS\n====================================\n");
	Sleep(15000);
	gInts->Engine->ClientCmd_Unrestricted("retry");

	return 0;
	//while(true)
	//{
	//	if(!gInts->Engine->IsConnected())
	//	{
	//		endTime = std::clock();

	//		ticks = endTime - startTime;
	//		Log::Console("%f seconds between vote and kick", ticks / (double)CLOCKS_PER_SEC);
	//		threadCreated = 0;
	//		return 0;
	//	}
	//}
}

const char *CNokick::name() const { return "KICK PREVENTION"; }

void CNokick::processCommand(CUserCmd *pUserCmd)
{
	if(voteKick)
	{
		// kicks will always go through if yes votes are greater than no votes
		if(yesVote > noVote)
		{
			// retry connection
			voteKick = false;
			Log::Console("DISCONNECT THREAD!");
			CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)disconnectRetry, nullptr, 0, nullptr);
		}
	}

	return;
}

bool CNokick::userMessage(int type, bf_read &data)
{
	static Color c{255, 0, 0, 255};

	char issue[50], player[50];

	int team, initiator = 0;

	//Log::Console("User Message");

	switch(type)
	{
	// Intercept other messages...
	case 45: // CallVoteFailed
		gInts->Cvar->ConsoleColorPrintf(c, "--> CALL VOTE FAILED\n");
		break;

	case 46: // VoteStart

		gInts->Cvar->ConsoleColorPrintf(c, "--> VOTE START\n");

		team	  = data.ReadByte();
		initiator = data.ReadByte();

		data.ReadString(issue, 50);
		data.ReadString(player, 50);

		Log::Console("-ISSUE--PLAYER-");
		Log::Console("\"%s\"", issue);
		Log::Console("\"%s\"", player);

		// #TF_vote_kick_player_other"
		if(issue[9] == 'k')
		{
			// check if the name is the same as our internal one or our server one
			if(gLocalPlayerVars.name == player || std::string(gLocalPlayerVars.info.name) == player)
			{
				// there always starts with 1 vote each
				// accuser has 1 yes vote and defender has 1 no vote
				//startTime = std::clock();
				yesVote = noVote = 1;
				teamPlayers = getNumOfPlayers();
				voteKick	= true;
			}
		}

		break;
	case 47: // VotePass

		gInts->Cvar->ConsoleColorPrintf(c, "--> VOTE PASS!\n");

		voteKick = false;

		initiator = data.ReadByte();
		data.ReadString(issue, 50);
		data.ReadString(player, 50);

		Log::Console("init: %d, issue: %s, player %s", initiator, issue, player);

		// #TF_vote_passed_kick_player
		// does happen clientside
		if(issue[16] == 'k')
		{
			// check to see if that persons first character matches ours
			if(gLocalPlayerVars.name == player || std::string(gLocalPlayerVars.info.name) == player)
			{
				// we need to use execure client cmd in order to get it to execute instantly
				Log::Console("--> retry now!!");
				gInts->Engine->ExecuteClientCmd("retry");
			}
		}

		break;

	case 48: // VoteFailed
		gInts->Cvar->ConsoleColorPrintf(c, "--> VOTE FAILED!\n");

		voteKick = false;

		return 0;

		break;
	default:
		// Log::Console("type: %d", type);
		break;
	}

	return true;
}

bool CNokick::emitSound(const char *sound)
{
	//Log::Console("sound: %s", sound);

	if(voteKick)
	{
		Log::Console("VOTEKICK::EMITSOUND(\"%s\")", sound);
		if(sound[0] == 'U' || sound[0] == 'u')
		{
			int len = strlen(sound);

			if(len > 4)
			{
				gInts->Cvar->ConsoleColorPrintf(Color{255, 0, 0, 255}, "--> SOUND: %s\n", sound);

				// we dont hear this if we are being kicked
				if(strstr(sound, "vote_success.wav")) // vote_success.wav
				{
					// vote passed - we're probably screwed anyway, but it cant hurt not to try.
					voteKick = false;
					// gInts->Engine->ExecuteClientCmd("retry");
					CreateThread(0, 0, disconnectRetry, 0, 0, 0);
				}
				else if(strstr(sound, "vote_yes.wav")) // vote_yes.wav
				{
					yesVote++;
					//startTime = std::clock();
				}
				else if(strstr(sound, "vote_no.wav")) // vote_no.wav
				{
					noVote++;
					//startTime = std::clock();
				}

				Log::Console("No votes: %d", noVote);
				Log::Console("Yes votes: %d", yesVote);
			}
		}
	}

	return true;
}

// get the current number of players on our team
int CNokick::getNumOfPlayers()
{
	int ret = 0;

	for(int i = 0; i < gInts->EntList->GetHighestEntityIndex(); i++)
	{
		CEntity<> ent{i};

		if(ent.isNull())
			continue;
		if(ent->IsDormant())
			continue;
		if(ent->GetClientClass()->iClassID != classId::CTFPlayer)
			continue;

		// see whether that person is on OUR team
		if(ent.get<int>(gEntVars.iTeam) == gLocalPlayerVars.team)
			ret++;
	}

	gInts->Cvar->ConsoleColorPrintf(Color{255, 0, 0, 255}, "--> WE HAVE GOT %d PLAYERS ON OUR TEAM\n", ret);

	return ret;
}

void CNokick::playSound(const char *soundName)
{

	Log::Console("Playsound with sound %s", soundName);

	typedef void(__thiscall * OriginalFn)(PVOID, const char *);
	auto &hook = VMTManager::GetHook(gInts->Surface);
	return hook.GetMethod<OriginalFn>(gOffsets.playSound)(gInts->Surface, soundName);
}
