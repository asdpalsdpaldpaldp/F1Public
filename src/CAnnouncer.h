#pragma once

#include "baseHeaders.h"

#include "IHack.h"

// this is gir's stuff mixed with the magic of IHack

class CAnnouncer : public IHack<CAnnouncer>, public IGameEventListener2
{
public:
	CAnnouncer()
	{
		killStreakTimeout = 4.0f;
		memset(chKillstreakSound, 0, sizeof(chKillstreakSound));
	}

	// IGameEventListener2 inherits
	void FireGameEvent(IGameEvent *pEvent) override;

	// IHack inherits
	void init();

	const char *name() const
	{
		return "ANNOUNCER";
	}

	void processCommand(CUserCmd *pUserCmd);

private:
	void PlaySound(const char *soundFile);
	char chKillstreakSound[50];

protected:
	float lastKillTime;
	int killCounter;
	int killStreakCounter;
	float killStreakTimeout;
};

extern CAnnouncer gAnnouncer;
