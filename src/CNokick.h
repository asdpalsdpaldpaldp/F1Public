#pragma once

#include "IHack.h"
#include "SDK.h"

class CNokick : public IHack
{

	//VMTBaseManager *playSoundHook;
	//VMTBaseManager *emitSoundHook;

public:
	// has a vote kick been called on us?
	bool voteKick;
	int yesVote, noVote;

	int teamPlayers;

	CNokick();

	static CNokick *getInst()
	{
		static CNokick inst;
		return &inst;
	}

	// Inherited via IHack
	const char *name() const override;

	void processCommand(CUserCmd *pUserCmd) override;

	bool userMessage(int type, bf_read &data);

	void init() override;

	bool emitSound(const char *sound);

	void playSound(const char *soundName);

private:
	// get the current number of players on our team
	int getNumOfPlayers();
};
