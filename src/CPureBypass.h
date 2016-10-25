#pragma once

#include "IHack.h"

#include "SDK.h"

class CPureBypass : public IHack<CPureBypass>
{
	DWORD dwPureLoc = NULL;

	VMTBaseManager *demoPlayerHook;

public:
	CPureBypass() { demoPlayerHook = nullptr; };
	~CPureBypass() { delete demoPlayerHook; };

	const char *name() const;

	void init();

	bool paint(/*CUserCmd **/);
};

extern CPureBypass gPureBypass;