#include "CMisc.h"

#include "SDK.h"
#include "CEntity.h"

#include "Util.h"

const char *CMisc::name() const
{
	return "MISC ACTIONS";
}

void CMisc::processCommand(CUserCmd *pUserCmd)
{
	CEntity<> local{me};

	if(local.isNull())
		return;

	if( bunnyHop->getValue() == true )
	{
		//if(pUserCmd->buttons & IN_JUMP)
		//{
		//	if(!(local.get<int>(gEntVars.iFlags) & FL_ONGROUND))
		//		pUserCmd->buttons &= ~IN_JUMP;
		//
		//	return true;
		//}

		static bool firstjump = 0, fakejmp;

		if( pUserCmd->buttons & IN_JUMP )
			if( !firstjump )
				firstjump = fakejmp = 1;
			else if( !( gLocalPlayerVars.flags & FL_ONGROUND ) )
				if( fakejmp )
					fakejmp = 0;
				else
					pUserCmd->buttons &= ~IN_JUMP;
			else
				fakejmp = 1;
		else
			firstjump = 0;
	}

	if( tauntSlide->getValue() == true )
	{
		if( local.get<int>( gEntVars.iPlayerCond ) & tf_cond::TFCond_Taunting )
			local.set<int>( gEntVars.iPlayerCond, local.get<int>( gEntVars.iPlayerCond ) & ~tf_cond::TFCond_Taunting );

		return;
	}

	//if(autoTaunt)
	//{
	//	// we do this here.... for reasons

	//	static bool congaTime = false;

	//	if(tf_roundstates::_from_integral(m_iRoundState) == +tf_roundstates::STATE_PREROUND)
	//	{
	//		congaTime = true;
	//		if((gLocalPlayerVars.flags & tf_cond::TFCond_Taunting) == false)
	//		{
	//			gInts.Cvar->ConsoleColorPrintf({255,0,0,255}, "---> DO THE CONGA!!!\n");
	//			gInts.Engine->ClientCmd_Unrestricted("taunt_by_name Taunt: Rock, Paper, Scissors");
	//		}
	//	}
	//	else if(congaTime == true)
	//	{
	//		congaTime = false;
	//		gInts.Engine->ClientCmd_Unrestricted("stop_taunt");
	//	}
	//}

	if( local.get<int>( gEntVars.iFov ) != fovChanger->getValue() )
	{
		if( ( gLocalPlayerVars.cond & tf_cond::TFCond_Zoomed ) == false )
			local.set<int>( gEntVars.iFov, fovChanger->getValue() );
	}

	static ConVar *pNoPush = gInts.Cvar->FindVar( "tf_avoidteammates_pushaway" );

	if( pNoPush != NULL )
	{
		pNoPush->SetValue( noPush->getValue() );
	}

	if( alwaysAttack2->getValue() )
	{
		pUserCmd->buttons |= IN_ATTACK2;
	}

	return;
}

bool CMisc::processEntity( int index )
{

	CEntity<> ent{ index };

	if( ent.isNull() )
		return false;

	if( ent->IsDormant() )
		return false;

	if( ent->GetClientClass()->iClassID != classId::CTFPlayer )
		return false;

	if(removeDisguise->getValue() )
	{
		tf_cond curCond = ent.get<tf_cond>( gEntVars.iPlayerCond );

		if( curCond & tf_cond::TFCond_Disguised )
			ent.set<int>( gEntVars.iPlayerCond, curCond & ~tf_cond::TFCond_Disguised );
	}

	return true;
}

void CMisc::menuUpdate( F1_IConVar ** menuArray, int & currIndex )
{
	menuArray[ currIndex++ ] = miscSwitch;

	if( miscSwitch->getValue() )
	{
		menuArray[ currIndex++ ] = bunnyHop;
		menuArray[ currIndex++ ] = tauntSlide;
		menuArray[ currIndex++ ] = fovChanger;
		menuArray[ currIndex++ ] = removeDisguise;
		menuArray[ currIndex++ ] = alwaysAttack2;
		menuArray[ currIndex++ ] = noPush;
	}
}
