#pragma once

#include <string>
#include <vector>

#include "bf_buffer.h"

#include "F1_ConVar.h"

//#define F1_ENABLE_CONSOLE_VARS

enum class ButtonCode_t;

class CUserCmd;

enum class type_t
{
	Bool = 0,
	Int,
	Float,
	Switch, // uses bool to decide whether it is open or closed but also has a vector of vars inside of it
	Enum, // uses BETTER_ENUM
};

// base hack interface
class IHack
{
public:

	virtual ~IHack()
	{
	}

	// each hack must impl the name function
	virtual const char *name() const
	{
		return "unassigned";
	}

	// called on init
	virtual void init()
	{
		return;
	}

	// called during paint traverse
	virtual bool paint()
	{
		return false;
	}

	// called during clientmode createmove - takes a CUserCMD
	virtual void processCommand(CUserCmd *pUserCmd)
	{
		return;
	}

	// THIS IS DISABLED
	// RUN EVERYTHING IN move(CUserCmd *pUserCmd)
	//virtual bool chlmove(CUserCmd *pUserCmd)
	//{
	//	return false;
	//}

	// called during keyevent
	virtual bool keyEvent(ButtonCode_t keynum)
	{
		return false;
	}

	virtual bool processEntity(int index)
	{
		return false;
	}

	virtual void menuUpdate( F1_IConVar **menuArray, int &currIndex )
	{
		return;
	}
};