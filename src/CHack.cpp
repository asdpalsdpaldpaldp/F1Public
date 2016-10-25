#include "CHack.h"
#include "Panels.h"
#include "CDrawManager.h"
#include "Util.h"
#include "CHackState.h"

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

DEFINE_RECURSE_CALL_FUNCTION_NO_ARGS( init );

CHack::CHack() : menu( 100, 100, 200 )
{
	// 0 out hack array
	//memset( pHackArray, 0, sizeof( pHackArray ) );

	//auto radar = new CRadar();

	// for ease of use
	//int currIndex = 0;

	// TODO add hacks here
	//pHackArray[ currIndex++ ] = new CESP();
	////pHackArray[ currIndex++ ] = new CGlow();
	//pHackArray[ currIndex++ ] = new CAimbot();
	//pHackArray[ currIndex++ ] = radar;
	//pHackArray[ currIndex++ ] = new CAntiaim();
	//pHackArray[ currIndex++ ] = new CAntiSmac();
	//pHackArray[ currIndex++ ] = new CPureBypass();
	//pHackArray[ currIndex++ ] = new CMisc();
	//pHackArray[ currIndex++ ] = new CBackstab();
	//pHackArray[ currIndex++ ] = new CTrigger();
	//pHackArray[ currIndex++ ] = new CAutoAirblast();
	//pHackArray[ currIndex++ ] = this;
	//pHackArray[ currIndex++ ] = &gPlayerManager;
	//pHackArray[ currIndex++ ] = new CAnnouncer();
	//pHackArray[ currIndex++ ] = new CPureBypass();

	windowArray.push_back( &menu );
	windowArray.push_back( &gRadar );

	// set up our convars
	hackSwitch = new F1_ConVar<Switch>( "Hack", false );
	fakeLag = new F1_ConVar<bool>( " - Fake lag", false );
	fakeLagAmount = new F1_ConVar<int>( " - - Amount", 15, 1, 100, 1 );
	fakeCrouch = new F1_ConVar<int>( " - Fake crouch", 0, 0, 2, 1 );
	tickCountConstant = new F1_ConVar<int>( " - Tick count", 0, -2000, 2000, 1 );
	speedHackSpeed = new F1_ConVar<int>( " - Speedhack speed", 7, 0, 100, 1 );
	speedHackConstant = new F1_ConVar<bool>( " - Force speedhack", false );
}
//===================================================================================

CHack::~CHack()
{
}
//===================================================================================

// set up draw manager and netvars
void CHack::intro()
{
	_TRY
	{

		// wait until we have completely inited before running intro
		while(!HState::instance()->checkStatus(HState::FullyInited))
		{
			HState::instance()->think();
		}

		// creds lmaobox
		//if(gInts.steam.steamApps->BIsDlcInstalled(459)) // http://steamdb.info/app/459/
		//{
		//	Log::Error("Why are you trying to run a public hack on a premium account??\nIm warning you now!\nARE YOU SURE YOU WANT TO GET VAC BANNED??");
		//}

		gDrawManager.Initialize(); //Initalize the drawing class.

		gEntVars.find(); // find our netvars

		// find gameResource offsets
		// these are seperate from the entVars due to the number of
		// netvars that there are
		//gInts.GameResource->findOffsets();

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
			RecurseCallinit( ACTIVE_HACKS );
		}
		_CATCH_SEH_REPORT_ERROR( this, RecurseCallinit );

		// intro printing stuff to console

		Color c(0, 0, 255, 255);
		gInts.Cvar->ConsolePrintf(XorString("________________________________________\n"));
		gInts.Cvar->ConsoleColorPrintf(c, XorString("  __| _ | _  )    \\ _ |   /   \n"));
		gInts.Cvar->ConsoleColorPrintf(c, XorString("  _|    |   /   (  |  |   _ \\ \n"));
		gInts.Cvar->ConsoleColorPrintf(c, XorString(" _|    _| ___| \\__/  _| \\___/\n"));
		gInts.Cvar->ConsoleColorPrintf(c, XorString("Fissi0N's F12016 hack loaded successfully.\n"));
		gInts.Cvar->ConsoleColorPrintf(c, XorString("Have Fun!\n"));
		gInts.Cvar->ConsolePrintf(XorString("________________________________________\n"));

		CSteamID localID = gInts.steam.user->GetSteamID();

		gInts.Cvar->ConsoleColorPrintf(c, "SteamID:     %s\n", localID.Render());
		gInts.Cvar->ConsoleColorPrintf(c, "newSteamID:  %s\n", localID.SteamRender());
		gInts.Cvar->ConsoleColorPrintf(c, "PersonaName: %s\n", gInts.steam.friends->GetPersonaName());
		gInts.Cvar->ConsolePrintf(XorString("________________________________________\n"));

		// unprotect / remove mins and maxes of all cvars
		// causes plenty of problems on some smac servers
		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)killCvars, nullptr, 0, nullptr);

		while( ( gInts.thisWindow = FindWindow( "Valve001", NULL ) ) == false ) Sleep( 200 );

		if( gInts.thisWindow )
			gInts.oldWindowProc = ( WNDPROC ) SetWindowLongPtr( gInts.thisWindow, GWLP_WNDPROC, ( LONG_PTR ) Hooked_WndProc );

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
void CHack::menuUpdate( F1_IConVar ** menuArray, int & currIndex )
{

	menuArray[ currIndex++ ] = hackSwitch;
	
	if(hackSwitch->getValue() )
	{
		menuArray[ currIndex++ ] = fakeLag;
		if( fakeLag->getValue() )
		{
			menuArray[ currIndex++ ] = fakeLagAmount;
		}
		menuArray[ currIndex++ ] = fakeCrouch;
		menuArray[ currIndex++ ] = tickCountConstant;
		menuArray[ currIndex++ ] = speedHackSpeed;
		menuArray[ currIndex++ ] = speedHackConstant;
	}

	return;
}
