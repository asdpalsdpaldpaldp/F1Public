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
template<typename Derived>
class IHack
{

private:

	Derived &derived()
	{
		return static_cast< Derived& >( *this );
	}

public:

	~IHack()
	{}

	// each hack must impl the name function

	// These functions must be defined in doubles to allow the derived class
	// to not implement them
	const char *_name() const
	{
		return derived().name();
	}
	const char *name() const
	{
		return derived().name();
	}

	// called on init
	void _init()
	{
		return derived().init();
	}
	void init()
	{
		return;
	}

	// called during paint traverse
	bool _paint()
	{
		return derived().paint();
	}
	bool paint()
	{
		return false;
	}

	void _processCommandBeforePred(CUserCmd *pCommand)
	{
		return derived().processCommandBeforePred(pCommand);
	}
	void processCommandBeforePred(CUserCmd *pCommand)
	{
		return;
	}

	// called during clientmode createmove - takes a CUserCMD
	void _processCommand( CUserCmd *pUserCmd )
	{
		return derived().processCommand( pUserCmd );
	}
	void processCommand( CUserCmd *pUserCmd )
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
	bool _keyEvent( ButtonCode_t keynum )
	{
		return derived().keyEvent( keynum );
	}
	bool keyEvent( ButtonCode_t keynum )
	{
		return;
	}

	// the entity here will not be the local player
	// and it will not be null
	void _processEntity( CBaseEntity *pBaseEntity )
	{
		return derived().processEntity( pBaseEntity );
	}
	void processEntity( CBaseEntity *pBaseEntity )
	{
		return;
	}

	void _menuUpdate( F1_IConVar **menuArray, int &currIndex )
	{
		return derived().menuUpdate( menuArray, currIndex );
	}
	void menuUpdate( F1_IConVar **, int & )
	{
		return;
	}

};