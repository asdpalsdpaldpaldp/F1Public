#include <thread>

#include "CHack.h"
#include "Panels.h"
#include "CDrawManager.h"
#include "Util.h"

// include hacks
#include "CEsp.h"
#include "CBackstab.h"
#include "CGlow.h"
#include "CTrigger.h"
#include "CMisc.h"
//#include "CNoise.h"
#include "CAimbot.h"
#include "CAutoAirblast.h"
#include "CAnnouncer.h"
//#include "CNospread.h"
#include "CPureBypass.h"
//#include "CNokick.h"
#include "CRadar.h"
#include "CAntiaim.h"
#include "CAntiSmac.h"
#include "CPlayerManager.h"

#include "modules.h"

CScreenSize gScreenSize;

// this can be easily fixed by making gInts a pointer
CInterfaces *gInts;

DEFINE_RECURSE_CALL_FUNCTION_NO_ARGS(init);

PVOID tempCM;
PVOID clientModeThread()
{
	// the new not_null means that we need a temp here to help us
	do
	{
		DWORD dwClientModeAddress = gSignatures.GetClientSignature(XorString("8B 0D ? ? ? ? 8B 02 D9 05"));
		XASSERT(dwClientModeAddress);
		// get clientmode from 2 bits further into the pattern.
		//gInts->ClientMode = **reinterpret_cast<ClientModeShared ***>(dwClientModeAddress + 2);
		tempCM = **reinterpret_cast<void ***>(dwClientModeAddress + 0x2);
	} while(tempCM == nullptr);

	// once we find it, assign it
	//gInts->ClientMode = static_cast<ClientModeShared *>(tempCM);

	// now that clientmode is not a nullptr we can hook it!

	//VMTBaseManager *clientModeHook = new VMTBaseManager(); //Setup our VMTBaseManager for ClientMode.
	//clientModeHook->Init(gInts->ClientMode);
	//clientModeHook->HookMethod(&Hooked_CreateMove, gOffsets.createMoveOffset); //ClientMode create move is called inside of CHLClient::CreateMove, and thus no need for hooking WriteUserCmdDelta.
	//clientModeHook->Rehook();
	return tempCM;
}

CSteamInterfaces getSteamInterfaces()
{
	// get the createinterface function
	auto steamClientFactory = srcFactory(GetProcAddress(gSignatures.GetModuleHandleSafe("SteamClient.dll"), "CreateInterface"));

	// get the steamclient interface
	auto client = steamClientFactory.get<ISteamClient017 *>(STEAMCLIENT_INTERFACE_VERSION_017, NULL);

	// connect to current user
	HSteamPipe hNewPipe = client->CreateSteamPipe();
	HSteamUser hNewUser = client->ConnectToGlobalUser(hNewPipe);

	return{client, reinterpret_cast<ISteamFriends002 *>(client->GetISteamFriends(hNewUser, hNewPipe, STEAMFRIENDS_INTERFACE_VERSION_002)),
		reinterpret_cast<ISteamUser017 *>(client->GetISteamUser(hNewUser, hNewPipe, STEAMUSER_INTERFACE_VERSION_017)),
		reinterpret_cast<ISteamUserStats011 *>(client->GetISteamUserStats(hNewUser, hNewPipe, STEAMUSERSTATS_INTERFACE_VERSION_011))};
}

CHack::CHack() try : menu(100, 100, 200),
		ClientFactory(GetProcAddress(gSignatures.GetModuleHandleSafe("client.dll"), "CreateInterface")),
		EngineFactory(GetProcAddress(gSignatures.GetModuleHandleSafe("engine.dll"), "CreateInterface")),
		VGUIFactory(GetProcAddress(gSignatures.GetModuleHandleSafe(XorString("vguimatsurface.dll")), XorString("CreateInterface"))),
		VGUI2Factory(GetProcAddress(gSignatures.GetModuleHandleSafe(XorString("vgui2.dll")), XorString("CreateInterface"))),
		MaterialSystemFactory(GetProcAddress(gSignatures.GetModuleHandleSafe("MaterialSystem.dll"), "CreateInterface")),
		PhysicsFactory(GetProcAddress(gSignatures.GetModuleHandleSafe("vphysics.dll"), "CreateInterface")),
		CvarFactory(GetProcAddress(gSignatures.GetModuleHandleSafe(XorString("vstdlib.dll")), XorString("CreateInterface"))),
		interfaces(
			// == CLIENT ==

			[this]() -> auto { auto c = ClientFactory.get<CHLClient *>("VClient017");
				gHookManager.hookMethod(c, gOffsets.createMoveOffset, &CHack::Hooked_CHLCreateMove); 
				gHookManager.hookMethod(c, gOffsets.keyEvent, &CHack::Hooked_KeyEvent);
				return c; }(),

			ClientFactory.get<CEntList *>("VClientEntityList003"),

			ClientFactory.get<CPrediction *>("VClientPrediction001"),

			ClientFactory.get<IGameMovement *>("GameMovement001"),

			[this]() -> auto { auto cm = static_cast<ClientModeShared *>(clientModeThread()); 
				gHookManager.hookMethod(cm, gOffsets.createMoveOffset, &CHack::Hooked_CreateMove);
				return cm; }(),

			// == ENGINE ==

			EngineFactory.get<EngineClient *>("VEngineClient013"),

			EngineFactory.get<CModelInfo *>("VModelInfoClient006"),

			EngineFactory.get<CModelRender *>("VEngineModel016"),

			EngineFactory.get<CRenderView *>("VEngineRenderView014"),

			EngineFactory.get<CEngineTrace *>("EngineTraceClient003"),

			EngineFactory.get<CUniformRandomStream *>("VEngineRandom001"),

			EngineFactory.get<IGameEventManager2 *>("GAMEEVENTSMANAGER002"),

			EngineFactory.get<IVDebugOverlay *>("VDebugOverlay003"),

			EngineFactory.get<IEngineSound *>("IEngineSoundClient003"),

			*reinterpret_cast<CDemoPlayer **>(gSignatures.GetEngineSignature("8B 0D ? ? ? ? 50 FF 56 14") + 0x2),

			*reinterpret_cast<CClientState **>(gSignatures.GetEngineSignature("B9 ? ? ? ? E8 ? ? ? ? 83 F8 FF 5E") + 0x1),

			// == VGUI ==
			VGUIFactory.get<ISurface *>("VGUI_Surface030"),

			// == VGUI2 ==
			VGUI2Factory.get<IPanel *>("VGUI_Panel009"),

			// == VSTDLIB ==
			CvarFactory.get<ICvar *>("VEngineCvar004"),

			*reinterpret_cast<CGlobals **>(gSignatures.GetEngineSignature("A1 ? ? ? ? 8B 11 68") + 8),

			// == VPHYSICS ==
			PhysicsFactory.get<IPhysicsSurfaceProps *>("VPhysicsSurfaceProps001"),

			// == MATERIALSYSTEM ==
			MaterialSystemFactory.get<CMaterialSystem *>("VMaterialSystem081"),

			getSteamInterfaces()

		)
{
	windowArray.push_back(&menu);
	windowArray.push_back(&gRadar);

	gInts = &interfaces;

	gHookManager.hookMethod(gInts->Input, gOffsets.getUserCmdOffset, &CHack::Hooked_GetUserCmd);

	// always hook panels last!
	gHookManager.hookMethod(gInts->Panels, gOffsets.paintTraverseOffset, &CHack::Hooked_PaintTraverse);

	canIntro = true;
}
catch(...)
{
	Log::Fatal("Failed CHack::CHack()");
}

CHack::~CHack()
{}

void CHack::Init(HINSTANCE hInstance)
{
	_INSTALL_SEH_TRANSLATOR();

	// spawn these threads first
	//std::thread{clientModeThread}.detach();

	/*

	if(gInts == nullptr) //Prevent repeat callings.
	{
		// get the factories
		auto ClientFactory = srcFactory(GetProcAddress(gSignatures.GetModuleHandleSafe("client.dll"), "CreateInterface"));
		auto EngineFactory = srcFactory(GetProcAddress(gSignatures.GetModuleHandleSafe("engine.dll"), "CreateInterface"));
		auto VGUIFactory = srcFactory(GetProcAddress(gSignatures.GetModuleHandleSafe(XorString("vguimatsurface.dll")), XorString("CreateInterface")));
		auto VGUI2Factory = srcFactory(GetProcAddress(gSignatures.GetModuleHandleSafe(XorString("vgui2.dll")), XorString("CreateInterface")));
		auto MaterialSystemFactory = srcFactory(GetProcAddress(gSignatures.GetModuleHandleSafe("MaterialSystem.dll"), "CreateInterface"));
		auto physicsFactory = srcFactory(GetProcAddress(gSignatures.GetModuleHandleSafe("vphysics.dll"), "CreateInterface"));
		auto CvarFactory = srcFactory(GetProcAddress(gSignatures.GetModuleHandleSafe(XorString("vstdlib.dll")), XorString("CreateInterface")));


		do {} while(tempCM == nullptr);

		// TODO change this if we use its
		CreateInterfaceFn AppSysFactory = nullptr;
		DWORD dwAppSystem = gSignatures.GetEngineSignature("A1 ? ? ? ? 8B 11 68");
		XASSERT(dwAppSystem);
		if(dwAppSystem)
		{

			DWORD dwAppSystemAddress = **reinterpret_cast<PDWORD *>((dwAppSystem)+1);
			AppSysFactory = reinterpret_cast<CreateInterfaceFn>(dwAppSystemAddress);
		}

		DWORD dwDemoPlayer = gSignatures.GetEngineSignature("8B 0D ? ? ? ? 50 FF 56 14") + 0x2;

		DWORD dwClientState = gSignatures.GetEngineSignature("B9 ? ? ? ? E8 ? ? ? ? 83 F8 FF 5E") + 0x1;


		// initialise gInts
		::gInts = new CInterfaces(
			// == CLIENT ==

			[&ClientFactory]() -> auto {auto c = ClientFactory.get<CHLClient *>("VClient017");
				gHookManager.hookMethod(c, gOffsets.createMoveOffset, &CHack::Hooked_CHLCreateMove); return c; }(),

			ClientFactory.get<CEntList *>("VClientEntityList003"),

			ClientFactory.get<CPrediction *>("VClientPrediction001"),

			ClientFactory.get<IGameMovement *>("GameMovement001"),

			static_cast<ClientModeShared *>(tempCM),

			// == ENGINE ==

			EngineFactory.get<EngineClient *>("VEngineClient013"),

			EngineFactory.get<CModelInfo *>("VModelInfoClient006"),

			EngineFactory.get<CModelRender *>("VEngineModel016"),

			EngineFactory.get<CRenderView *>("VEngineRenderView014"),

			EngineFactory.get<CEngineTrace *>("EngineTraceClient003"),

			EngineFactory.get<CUniformRandomStream *>("VEngineRandom001"),

			EngineFactory.get<IGameEventManager2 *>("GAMEEVENTSMANAGER002"),

			EngineFactory.get<IVDebugOverlay *>("VDebugOverlay003"),

			EngineFactory.get<IEngineSound *>("IEngineSoundClient003"),

			*reinterpret_cast<CDemoPlayer **>(dwDemoPlayer),

			*reinterpret_cast<CClientState **>(dwClientState),

			// == VGUI ==
			VGUIFactory.get<ISurface *>("VGUI_Surface030"),

			// == VGUI2 ==
			VGUI2Factory.get<IPanel *>("VGUI_Panel009"),

			// == VSTDLIB ==
			CvarFactory.get<ICvar *>("VEngineCvar004"),

			*reinterpret_cast<CGlobals **>(dwAppSystem + 8),

			// == VPHYSICS ==
			physicsFactory.get<IPhysicsSurfaceProps *>("VPhysicsSurfaceProps001"),

			// == MATERIALSYSTEM ==
			MaterialSystemFactory.get<CMaterialSystem *>("VMaterialSystem081"),

			getSteamInterfaces()

			);

		gHookManager.hookMethod(gInts->Client, gOffsets.keyEvent, &CHack::Hooked_KeyEvent);

		gHookManager.hookMethod(gInts->ClientMode, gOffsets.createMoveOffset, &CHack::Hooked_CreateMove);

		// TODO remove hardcodedness
		//gHookManager.hookMethod(gInts->ModelRender, 19, &CHack::Hooked_DrawModelExecute);

		// hook getusercmd from CInput
		//VMTBaseManager *inputHook = new VMTBaseManager();
		//inputHook->Init(gInts->Input);
		//inputHook->HookMethod(&Hooked_GetUserCmd, gOffsets.getUserCmdOffset);
		//inputHook->Rehook();

		gHookManager.hookMethod(gInts->Input, gOffsets.getUserCmdOffset, &CHack::Hooked_GetUserCmd);

		// hook panels last so that intro always happens after interfaces are ready
		gHookManager.hookMethod(gInts->Panels, gOffsets.paintTraverseOffset, &CHack::Hooked_PaintTraverse);
	}

	*/
	gInts->thisDll = hInstance;
	return; //The thread has been completed, and we do not need to call anything once we're done. The call to Hooked_PaintTraverse is now our main thread.
}

// set up draw manager and netvars
void CHack::intro()
{
	_TRY
	{
		
		do 
		{
			// wait for clientmode
		} while(canIntro == false);

		gDrawManager.Initialize(); //Initalize the drawing class.

		gEntVars.find(); // find our netvars

		// find gameResource offsets
		// these are seperate from the entVars due to the number of
		// netvars that there are
		//gInts->GameResource->findOffsets();

		LOGDEBUG(XorString("==========================================================="));

		//{
		//	#include "CDumper.h"
		//	CDumper nDumper;
		//	nDumper.SaveDump( );
		//}

		// run their init function (this could be rolled into the constructor?
		//int i = 0;
		//auto *pHack = gHack.pHackArray[ i++ ];
		//while( pHack != NULL )
		//{
		//	_TRY
		//	{
		//		pHack->init();
		//		pHack = pHackArray[ i++ ];
		//	}
		//	_CATCH_SEH_REPORT_ERROR( pHack, init() );

		//}
		_TRY
		{
			RecurseCallinit(ACTIVE_HACKS);
		}
		_CATCH_SEH_REPORT_ERROR(this, RecurseCallinit);

		// intro printing stuff to console

		Color c(0, 0, 255, 255);
		gInts->Cvar->ConsolePrintf(XorString("________________________________________\n"));
		gInts->Cvar->ConsoleColorPrintf(c, XorString("  __| _ | _  )    \\ _ |   /   \n"));
		gInts->Cvar->ConsoleColorPrintf(c, XorString("  _|    |   /   (  |  |   _ \\ \n"));
		gInts->Cvar->ConsoleColorPrintf(c, XorString(" _|    _| ___| \\__/  _| \\___/\n"));
		gInts->Cvar->ConsoleColorPrintf(c, XorString("Fissi0N's F12016 hack loaded successfully.\n"));
		gInts->Cvar->ConsoleColorPrintf(c, XorString("Have Fun!\n"));
		gInts->Cvar->ConsolePrintf(XorString("________________________________________\n"));

		CSteamID localID = gInts->steam.user->GetSteamID();

		gInts->Cvar->ConsoleColorPrintf(c, "SteamID:     %s\n", localID.Render());
		gInts->Cvar->ConsoleColorPrintf(c, "newSteamID:  %s\n", localID.SteamRender());
		gInts->Cvar->ConsoleColorPrintf(c, "PersonaName: %s\n", gInts->steam.friends->GetPersonaName());
		gInts->Cvar->ConsolePrintf(XorString("________________________________________\n"));

		// unprotect / remove mins and maxes of all cvars
		// causes plenty of problems on some smac servers
		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)killCvars, nullptr, 0, nullptr);

		while((gInts->thisWindow = FindWindow("Valve001", NULL)) == false) Sleep(200);

		if(gInts->thisWindow)
			gInts->oldWindowProc = (WNDPROC)SetWindowLongPtr(gInts->thisWindow, GWLP_WNDPROC, (LONG_PTR)Hooked_WndProc);

		// finally log that we successfully hooked and injected
		Log::Msg(XorString("Injection Successful")); //If the module got here without crashing, it is good day.

		//ConVar_Register(0);
	}
	_CATCH
	{
		Log::Fatal("Undefined exception during intro!\nExiting now!");
	}
}
//===================================================================================
void CHack::menuUpdate(F1_IConVar ** menuArray, int & currIndex)
{
	menuArray[currIndex++] = &hackSwitch;

	if(hackSwitch.getValue())
	{
		menuArray[currIndex++] = &fakeLag;
		if(fakeLag.getValue())
		{
			menuArray[currIndex++] = &fakeLagAmount;
		}
		menuArray[currIndex++] = &fakeCrouch;
		menuArray[currIndex++] = &tickCountConstant;
		menuArray[currIndex++] = &speedHackSpeed;
		menuArray[currIndex++] = &speedHackConstant;
	}

	return;
}
