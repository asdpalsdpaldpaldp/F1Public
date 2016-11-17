#pragma once

#include "SDK.h"
#include "F1_Menu.h"

#ifdef __DEBUG
#include "CDumper.h"
#endif

// prepare to die
#define ACTIVE_HACKS \
	gAimbot, gAnnouncer, gAntiaim, *CAntiSmac::getInst(), \
	gAutoAirblast, /*gBackstab,*/ gEsp, /*gGlow, gNokick, */gPlayerManager, \
	gPureBypass, gRadar, gTrigger, gMisc, gHack

class CHack : public IHack<CHack>
{
	int createMoveEBP;

	// use of vector here allows for easy adding and removing of windows
	std::vector<F1_IComponent *> windowArray;

	F1_Menu menu;

	F1_ConVar<Switch> hackSwitch{"Hack", false};
	F1_ConVar<bool> fakeLag{" - Fake lag", false};
	F1_ConVar<int> fakeLagAmount{" - - Amount", 15, 1, 100, 1};
	F1_ConVar<int> fakeCrouch{" - Fake crouch", 0, 0, 2, 1};
	F1_ConVar<int> tickCountConstant{" - Tick count", 0, -200000, 200000, 100};
	F1_ConVar<int> speedHackSpeed{" - Speedhack speed", 7, 0, 100, 1};
	F1_ConVar<bool> speedHackConstant{" - Force speedhack", false};

	bool canIntro = false;

	srcFactory ClientFactory;
	srcFactory EngineFactory;
	srcFactory VGUIFactory;
	srcFactory VGUI2Factory;
	srcFactory MaterialSystemFactory;
	srcFactory PhysicsFactory;
	srcFactory CvarFactory;

	CInterfaces interfaces;

public:
	CHack();
	~CHack();

	// set up draw manager and netvars
	void intro();

	// setup the hack
	static void Init(HINSTANCE hInstance);

	static bool __fastcall Hooked_CreateMove( PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd* pCommand );

	static void __fastcall Hooked_CHLCreateMove( PVOID Client, int edx, int sequence_number, float input_sample_frametime, bool active );

	static void __fastcall Hooked_PaintTraverse( PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce );

	static int __fastcall Hooked_KeyEvent( PVOID CHLClient, int edx, int eventcode, ButtonCode_t keynum, const char *currentBinding );

	static CUserCmd *__fastcall Hooked_GetUserCmd( PVOID Input, int edx, int command_number );

	static void __stdcall Hooked_DrawModelExecute( void *state, ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld );

	static LRESULT __stdcall CHack::Hooked_WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	//inline IHack **getHackArray() { return pHackArray; };

	void menuUpdate( F1_IConVar **menuArray, int &currIndex );
};

extern CHack gHack;
