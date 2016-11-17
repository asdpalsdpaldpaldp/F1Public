#include "CMisc.h"

#include "SDK.h"
#include "CEntity.h"

#include "Util.h"

CMisc gMisc;

const char *CMisc::name() const
{
	return "MISC ACTIONS";
}

void CMisc::processCommandBeforePred(CUserCmd *pUserCmd)
{
	//CEntity<> local{me};
	auto *pLocalEntity = GetBaseEntity(me);

	if(pLocalEntity == NULL)
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

		//static bool firstjump = 0, fakejmp;

		//if( pUserCmd->buttons & IN_JUMP )
		//	if( !firstjump )
		//		firstjump = fakejmp = 1;
		//	else if( !( pLocalEntity->GetFlags() & FL_ONGROUND ) )
		//		if( fakejmp )
		//			fakejmp = 0;
		//		else
		//			pUserCmd->buttons &= ~IN_JUMP;
		//	else
		//		fakejmp = 1;
		//else
		//	firstjump = 0;

		if(pUserCmd->buttons & IN_JUMP)
		{
			if(!( pLocalEntity->GetFlags() & FL_ONGROUND ))
				pUserCmd->buttons &= ~IN_JUMP;
		}
	}

	if( pLocalEntity->GetFov() != fovChanger->getValue() )
	{
		if((gLocalPlayerVars.cond & tf_cond::TFCond_Zoomed) == false)
			pLocalEntity->SetFov(fovChanger->getValue());
	}

	static ConVar *pNoPush = gInts->Cvar->FindVar( "tf_avoidteammates_pushaway" );

	if( pNoPush != NULL )
	{
		pNoPush->SetValue( !noPush->getValue() );
	}

	if( alwaysAttack2->getValue() )
	{
		pUserCmd->buttons |= IN_ATTACK2;
	}

	return;
}

void CMisc::processEntity( CBaseEntity *pBaseEntity )
{

	//CEntity<> ent{ pBaseEntity };

	if( pBaseEntity->GetClientClass()->iClassID != classId::CTFPlayer )
		return;

	if(removeDisguise->getValue() )
	{
		int curCond = pBaseEntity->GetCond();

		if( curCond & tf_cond::TFCond_Disguised )
			pBaseEntity->SetCond( curCond & ~tf_cond::TFCond_Disguised );
	}

	return;
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
