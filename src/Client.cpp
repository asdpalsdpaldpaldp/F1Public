#include "SDK.h"

#include "Util.h"
#include "F1_Cache.h"

//============================================================================================
bool __fastcall CHack::Hooked_CreateMove(PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd *pCommand)
{

	void *createMoveEBP;

	__asm
	{
		mov createMoveEBP, ebp;
	}

	auto bSendPacket = (bool *)(****(BYTE*****)createMoveEBP) - 0x1;

	//bool bSendPacket = NULL;

	_TRY
	{
		_INSTALL_SEH_TRANSLATOR();

		auto &hook = VMTManager::GetHook( ClientMode );
		bool bReturn = hook.GetMethod<bool( __thiscall * )( PVOID, float, CUserCmd * )>( gOffsets.createMoveOffset )( ClientMode, input_sample_frametime, pCommand );

		CEntity<> local{ me };
		CBaseEntity *localEnt = local.castToPointer<CBaseEntity>();

		// this should NEVER happen
		if( local.isNull() )
			throw;

		// update our stats every game tick
		gLocalPlayerVars.Class = local.get<tf_classes>( gEntVars.iClass );
		gLocalPlayerVars.cond = local.get<int>( gEntVars.iPlayerCond );
		gLocalPlayerVars.condEx = local.get<int>( gEntVars.iPlayerCondEx );
		gLocalPlayerVars.health = local.get<int>( gEntVars.iHealth );
		gLocalPlayerVars.team = local.get<int>( gEntVars.iTeam );
		gLocalPlayerVars.flags = local.get<int>( gEntVars.iFlags );
		gLocalPlayerVars.cmdNum = pCommand->command_number;
		gLocalPlayerVars.info = gInts.Engine->GetPlayerInfo( me );

		CBaseEntity *pLocalWep = gInts.EntList->GetClientEntity( HANDLE2INDEX( local.get<int>( gEntVars.hActiveWeapon ) ) );

		// get the current weapon tag for our active weapon
		gLocalPlayerVars.wepTag = CEntTag( pLocalWep );

		if( pLocalWep )
		{
			gLocalPlayerVars.activeWeapon = pLocalWep->GetClientClass()->iClassID;

			gLocalPlayerVars.flNextAttack = CEntity<>{ pLocalWep->GetIndex() }.get<float>( gEntVars.flNextPrimaryAttack );
		}
		else
			gLocalPlayerVars.activeWeapon = static_cast< classId >( -1 );


		// begin local client cmd prediction
		gLocalPlayerVars.pred.oldOrigin = localEnt->GetAbsOrigin();

		CMoveData moveData;

		memset( &moveData, 0, sizeof( CMoveData ) );

		// back up the globals
		float oldCurTime = gInts.Globals->curtime;
		float oldFrameTime = gInts.Globals->frametime;

		// set up the globals
		gInts.Globals->curtime = local.get<float>( gEntVars.nTickBase ) * gInts.Globals->interval_per_tick;
		gInts.Globals->frametime = gInts.Globals->interval_per_tick;

		CBaseEntity *pLocal = local.castToPointer<CBaseEntity>();

		// set the current cmd
		local.set<CUserCmd *>( 0x107C, pCommand );

		gInts.GameMovement->StartTrackPredictionErrors( pLocal );

		// do actual player cmd prediction
		gInts.Prediction->SetupMove( pLocal, pCommand, gInts.MoveHelper, &moveData );
		gInts.GameMovement->ProcessMovement( pLocal, &moveData );
		gInts.Prediction->RunCommand( pLocal, pCommand, gInts.MoveHelper );
		gInts.Prediction->FinishMove( pLocal, pCommand, &moveData );

		gInts.GameMovement->FinishTrackPredictionErrors( pLocal );

		// reset the current cmd
		local.set<CUserCmd *>( 0x107C, 0 );

		// restore the globals
		gInts.Globals->curtime = oldCurTime;
		gInts.Globals->frametime = oldFrameTime;

		gLocalPlayerVars.pred.origin = local->GetAbsOrigin();
		// end local entity prediction

		// set these before the hacks run
		// we cant have these in chlmove as by that point they have already run

		gHack.silentData.fMove = pCommand->forwardmove;
		gHack.silentData.sMove = pCommand->sidemove;
		gHack.silentData.view = pCommand->viewangles;

		//static bool oneTime = false;
		//if(!oneTime )
		//{
		//	allocConsole();
		//	generateItemList();
		//	oneTime = true;
		//}

		int i = 0;
		auto *pHack = gHack.pHackArray[ i++ ];
		while( pHack != NULL )
		{
			_TRY
			{
				pHack->processCommand( pCommand );
			}
			_CATCH_SEH_REPORT_ERROR( pHack, move() )
			pHack = gHack.pHackArray[ i++ ];
		}

		//bool *bSendPacket = ( bool * ) ( *( char ** ) ( gHack.createMoveEBP - 0x1 ) );

		NET_SetConVar setName( "name", gLocalPlayerVars.name.c_str() );
		gInts.Engine->GetNetChannelInfo()->SendNetMsg( *(INetMessage *)&setName );
	}
	_CATCHMODULE
	{
		Log::Error( "%s", e.what() );
	}
	_CATCH_SEH_REPORT_ERROR( &gHack, createMove() )

	return false;
}

void __fastcall CHack::Hooked_CHLCreateMove(PVOID CHLClient, int edx, int sequence_number, float input_sample_time, bool active)
{

	//sequence_number = 188;

	CUserCmd *pCommand = gInts.Input->GetUserCmd(sequence_number);

	if(!pCommand )
		return;

	

	// SPEEDHACK (only works on hvh servers)
	// creds gir
	// TODO move this into its own hack
	static int iSpeedCounter = 0; //Setup a global counter.
								  //If I'm pressing MOUSE4 and the counter was not 0.
	if( iSpeedCounter > 0 && ( GetAsyncKeyState( VK_LSHIFT ) || gHack.speedHackConstant->getValue() == true) )
	{
		iSpeedCounter--; //Decrement the counter.
		//pCommand->tick_count--; //Normalize tick_count.
		_asm
		{
			push eax; //Preserve EAX to the stack.
			mov eax, dword ptr ss : [ebp]; //Move EBP in to EAX.
			mov eax, [ eax ]; //Derefrence the base pointer.
			lea eax, [ eax + 0x4 ]; //Load the return address in to EAX.
			sub[ eax ], 0x5; //Make it return to -5 where it would normally.
			pop eax; //Restore EAX
		}
	}
	else
	{
		iSpeedCounter = gHack.speedHackSpeed->getValue(); //We want to run this 7 times.
	}

	int cmEBP = 0;

	// grab ebp
	__asm mov cmEBP, ebp;

	gHack.createMoveEBP = cmEBP;

	bool *bSendPacket = ( bool * ) ( *( char ** ) cmEBP - 0x1 );


	// TODO blow the bone cache first

	// manually interpolate each entity
	for( int i = 0; i < gInts.EntList->GetHighestEntityIndex(); i++ )
	{
		CBaseEntity *pBaseEntity = gInts.EntList->GetClientEntity( i );

		if(pBaseEntity != NULL )
			if(!pBaseEntity->IsDormant())
				pBaseEntity->Interpolate( gInts.Globals->curtime );
	}

	// reset the cache
	gCache.blow();

	VMTManager &hook = VMTManager::GetHook(CHLClient); // Get a pointer to the instance of your VMTManager with the function GetHook.
	hook.GetMethod<void(__thiscall *)(PVOID, int, float, bool)>(gOffsets.createMoveOffset)(CHLClient, sequence_number, input_sample_time,
		active); // Call the original.

	pCommand->command_number = 3599;

	pCommand->random_seed = CRandom::MD5_PseudoRandom( 3599 ) & 0x7fffffff;

	if( bCanShoot( CEntity<>{ me }, pCommand ) && ( CEntity<>{me}.get<BYTE>(gEntVars.iLifeState) == LIFE_ALIVE) )
	{
		*bSendPacket = false;
	}
	else if( !( pCommand->buttons & IN_ATTACK ) )
	{
		// only reset the angles if we are not attacking
		*bSendPacket = true;

		pCommand->viewangles = gHack.silentData.view;
		// gInts.Engine->SetViewAngles(silentData.view);

		// dont set these
		// pUserCmd->sidemove = silentData.sMove;
		// pUserCmd->forwardmove = silentData.fMove;

		// oh dear
		//gInts.ClientState->chokedcommands++;
	}
	else
	{
		*bSendPacket = true;
	}

	if( gHack.fakeLag->getValue() == true )
	{
		static int currLagIndex = 0;

		int maxLagIndex = gHack.fakeLagAmount->getValue();

		if( currLagIndex < maxLagIndex )
		{
			*bSendPacket = false;
			currLagIndex++;
		}
		else
		{
			*bSendPacket = true;
			currLagIndex = 0;
		}
	}
	else
	{
		*bSendPacket = true;
	}

	if( gHack.fakeCrouch->getValue() == 1 )
	{
		if( ( gInts.Globals->tickcount % 2 ) == 1 && pCommand->buttons & IN_DUCK )
		{
			*bSendPacket = false;

			pCommand->buttons &= ~IN_DUCK;
		}
	}

	pCommand->tick_count += gHack.tickCountConstant->getValue();

	// resign the cmd
	CVerifiedUserCmd *pSafeCommand = ( CVerifiedUserCmd * ) ( *( DWORD * ) ( gInts.Input.get() + 0xF8 ) + ( sizeof( CVerifiedUserCmd ) * ( sequence_number % 90 ) ) );
	pSafeCommand->m_cmd			  = *pCommand;
	pSafeCommand->m_crc			  = GetChecksumForCmd(pSafeCommand->m_cmd);
}
//============================================================================================
int __fastcall CHack::Hooked_KeyEvent(PVOID CHLClient, int edx, int eventcode, ButtonCode_t keynum, const char *currentBinding)
{
	int ret = 0;
	_TRY
	{
		_INSTALL_SEH_TRANSLATOR();

		VMTManager &hook = VMTManager::GetHook( CHLClient );																														// Get a pointer to the instance of your VMTManager with the function GetHook.
		ret = hook.GetMethod<int( __thiscall * )( PVOID, int, int, const char * )>( gOffsets.keyEvent )( CHLClient, eventcode, static_cast< int >( keynum ), currentBinding ); // Call the original.

		if( eventcode == 1 )
		{
			// TODO call each hacks's keyEvent code

			// call our menu's input handler
			if( gHack.menu.keyboardInput( keynum ) == 0 )
				ret = 0;
		}
	}
	_CATCH_SEH_REPORT_ERROR( &gHack, Hooked_KeyEvent() );

	return ret;
}

// no checks here ;)
CUserCmd *__fastcall CHack::Hooked_GetUserCmd(PVOID pInput, int edx, int sequence_number)
{
	return &(*(CUserCmd **)((DWORD)gInts.Input.get() + 0xF4))[sequence_number % 90];
}

void __stdcall CHack::Hooked_DrawModelExecute(void *state, ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld)
{

	auto &hook = VMTManager::GetHook(gInts.ModelRender);

	static bool active = false;

	if(pInfo.pModel)
	{
		std::string pszModelName = gInts.ModelInfo->GetModelName((DWORD *)pInfo.pModel);

		if(pszModelName.find("arms") != std::string::npos)
		{
			//Log::Console("mat name: %s", pszModelName.c_str());

			IMaterial* Hands = gInts.MatSystem->FindMaterial(pszModelName.c_str(), TEXTURE_GROUP_MODEL);
			Hands->SetMaterialVarFlag(MaterialVarFlags_t::MATERIAL_VAR_NO_DRAW, true);
			gInts.ModelRender->ForcedMaterialOverride(Hands, OverrideType_t::OVERRIDE_NORMAL);
		}
		//else if(pszModelName.find("models/player") != std::string::npos)
		//{
		//	try
		//	{
		//		IMaterial *mats[MAXSTUDIOSKINS];

		//		studiohdr_t *hdr = (studiohdr_t *)gInts.ModelInfo->GetStudiomodel((DWORD *)pInfo.pModel);

		//		CEntity<> other{pInfo.entity_index};

		//		if(other.isNull())
		//			goto exit; // please kill me

		//		CEntity<> local{me};

		//		if(other.get<BYTE>(gEntVars.iLifeState) == LIFE_ALIVE)
		//		{
		//			gInts.ModelInfo->GetModelMaterials((model_t *)pInfo.pModel, hdr->numtextures, mats);

		//			for(int i = 0; i < hdr->numtextures; i++)
		//			{
		//				IMaterial *mat = mats[i];

		//				if(!mat)
		//					continue;

		//				// Here we use SetMaterialVarFlag from the IMaterial class (31st index)
		//				mat->SetMaterialVarFlag(MaterialVarFlags_t::MATERIAL_VAR_IGNOREZ, true);
		//				mat->SetMaterialVarFlag(MaterialVarFlags_t::MATERIAL_VAR_FLAT, true);
		//				mat->SetMaterialVarFlag(MaterialVarFlags_t::MATERIAL_VAR_ALPHATEST, true);
		//				mat->AlphaModulate(1.0f); // don't need this, I just use it just cause.

		//				Color col = {0, 255, 255};
		//				
		//				float baseColor[4];

		//				col.getFloatArray(baseColor);

		//				gInts.RenderView->SetColorModulation(baseColor);
		//				// Call to our original DrawModelExecute (we could juat as easily call oDrawModelExecute).
		//				// We call the original so we can have different colors based on visibility
		//				hook.GetMethod<void(__thiscall *)(PVOID, void *, ModelRenderInfo_t &, matrix3x4 *)>(19)(gInts.ModelRender, state, pInfo, pCustomBoneToWorld);

		//				mat->SetMaterialVarFlag(MaterialVarFlags_t::MATERIAL_VAR_IGNOREZ, false);
		//				mat->SetMaterialVarFlag(MaterialVarFlags_t::MATERIAL_VAR_FLAT, false);
		//				mat->SetMaterialVarFlag(MaterialVarFlags_t::MATERIAL_VAR_ALPHATEST, false);
		//				mat->AlphaModulate(1.0f);

		//				Color col2 = {255, 255, 0};

		//				col2.getFloatArray(baseColor);
		//				gInts.RenderView->SetColorModulation(baseColor);

		//				hook.GetMethod<void(__thiscall *)(PVOID, void *, ModelRenderInfo_t &, matrix3x4 *)>(19)(gInts.ModelRender, state, pInfo, pCustomBoneToWorld);
		//				if(!active)
		//					active = true;
		//			}
		//		}
		//		else // draw normally
		//		{
		//			gInts.ModelInfo->GetModelMaterials((model_t *)pInfo.pModel, hdr->numtextures, mats);
		//			for(int i = 0; i < hdr->numtextures; i++)
		//			{
		//				IMaterial* mat = mats[i];
		//				if(!mat)
		//					continue;

		//				mat->SetMaterialVarFlag(MaterialVarFlags_t::MATERIAL_VAR_IGNOREZ, false);
		//				mat->SetMaterialVarFlag(MaterialVarFlags_t::MATERIAL_VAR_ALPHATEST, false);
		//				mat->SetMaterialVarFlag(MaterialVarFlags_t::MATERIAL_VAR_FLAT, false);

		//			}
		//		}
		//	}
		//	_CATCH_SEH_REPORT_ERROR(&gHack, drawModelExecuteOverridePlayer);
		//}
	}

	exit:
	// call the original

	if(!active)
	{
		hook.GetMethod<void(__thiscall *)(PVOID, void *, ModelRenderInfo_t &, matrix3x4 *)>(19)(gInts.ModelRender, state, pInfo, pCustomBoneToWorld);

	}
	gInts.ModelRender->ForcedMaterialOverride(NULL, OverrideType_t::OVERRIDE_NORMAL);
}


LRESULT __stdcall CHack::Hooked_WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	mouseButton mb = mouseButton::NONE;

	switch( uMsg )
	{
	case WM_LBUTTONDOWN:
		mb = mouseButton::LDOWN;
		break;
	case WM_LBUTTONUP:
		mb = mouseButton::LUP;
		break;
	case WM_RBUTTONDOWN:
		mb = mouseButton::RDOWN;
		break;
	case WM_RBUTTONUP:
		mb = mouseButton::RUP;
		break;
	case WM_MOUSEWHEEL:
		if( (int)wParam < 0 )
			mb = mouseButton::SCROLLDOWN;
		else
			mb = mouseButton::SCROLLUP;
		break;
	default: break;
	}

	if(mb != mouseButton::NONE )
	{

		int x, y;
		gInts.Surface->SurfaceGetCursorPos( x, y );

		// handle input
		for(auto &window : gHack.windowArray )
			window->handleMouseInput( x, y, mb );
	}

	return CallWindowProc( gInts.oldWindowProc, hWnd, uMsg, wParam, lParam );
}