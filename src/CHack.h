#pragma once

#include "SDK.h"
#include "F1_Menu.h"

#ifdef __DEBUG
#include "CDumper.h"
#endif

// prepare to die
#define ACTIVE_HACKS \
	gAimbot, gAnnouncer, gAntiaim, *CAntiSmac::getInst(), \
	gAutoAirblast, /*gBackstab,*/ gEsp, /*gGlow,*/ gNokick, gPlayerManager, \
	gPureBypass, gRadar, gTrigger, gMisc, gHack

class CHack : public IHack<CHack>
{
	int createMoveEBP;

	// use of vector here allows for easy adding and removing of windows
	std::vector<F1_IComponent *> windowArray;

	F1_Menu menu;

	struct pSilentData_t
	{
		int tickCounter = 0;

		float fMove, sMove;
		Vector view;
	};

	F1_ConVar<Switch> *hackSwitch;
	F1_ConVar<bool> *fakeLag;
	F1_ConVar<int> *fakeLagAmount;
	F1_ConVar<int> *fakeCrouch;
	F1_ConVar<int> *tickCountConstant;
	F1_ConVar<int> *speedHackSpeed;
	F1_ConVar<bool> *speedHackConstant;

public:
	CHack();
	~CHack();
	// for drawing on screen
	void paintTraverse(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce); // take all variables from parent...
	// set up draw manager and netvars
	void intro();

	static bool __fastcall Hooked_CreateMove( PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd* pCommand );

	static void __fastcall Hooked_CHLCreateMove( PVOID Client, int edx, int sequence_number, float input_sample_frametime, bool active );

	static void __fastcall Hooked_PaintTraverse( PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce );

	static int __fastcall Hooked_KeyEvent( PVOID CHLClient, int edx, int eventcode, ButtonCode_t keynum, const char *currentBinding );

	static CUserCmd *__fastcall Hooked_GetUserCmd( PVOID Input, int edx, int command_number );

	static void __stdcall Hooked_DrawModelExecute( void *state, ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld );

	static LRESULT __stdcall CHack::Hooked_WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	//inline IHack **getHackArray() { return pHackArray; };

	void menuUpdate( F1_IConVar **menuArray, int &currIndex );

	pSilentData_t silentData;
};

extern CHack gHack;
