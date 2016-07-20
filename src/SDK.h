#pragma once

//#define __DEBUG
//#define __XOR

#include "baseHeaders.h"

#include "Vector.h"
#include "CSignature.h"
#include "WeaponList.h"
#include "CGlobalVars.h"
#include "Log.h"
#include "Netvar.h"
#include "network.h"
#include "netmessage.h"
#include "keyval.h"
#include "prediction.h"
#include "bf_buffer.h"
#include "clientstate.h"
#include "random.h"
#include "IConvar.h"
#include "DebugOverlay.h"
#include "CHookManager.h"
#include "Material.h"
#include "CEtags.h"
#include "F1_Gui.h"
//#include "CPlayerResource.h"

class CGameTrace;
typedef CGameTrace trace_t;
struct Ray_t;
class CEngineTrace;
class IUniformRandomStream;
class CPlayerResource;

typedef void *(*InstallUniformRandomStreamFn)(IUniformRandomStream *pStream);

using namespace toolkit;

#pragma optimize("gsy", on)
#pragma warning(disable : 4244)			// Possible loss of data
#pragma comment(lib, "steamclient.lib") // use steamclient as thats where we are getting createinterface from!

typedef float matrix3x4[3][4];
//===================================================================================================
typedef struct player_info_s
{
	char name[32];
	int userID;
	char guid[33];
	unsigned long friendsID;
	char friendsName[32];
	bool fakeplayer;
	bool ishltv;
	unsigned long customFiles[4];
	unsigned char filesDownloaded;
} player_info_t;
//===================================================================================================
class CHLClient
{
public:
	ClientClass *GetAllClasses(void)
	{
		typedef ClientClass *(__thiscall * OriginalFn)(PVOID); // Anything inside a VTable is a __thiscall unless it completly disregards the thisptr. You can
															   // also call them as __stdcalls, but you won't have access to the __thisptr.
		return getvfunc<OriginalFn>(this, 8)(this);			   // Return the pointer to the head CClientClass.
	}
};
//===================================================================================================
class CGlobals
{
public:
	float realtime;
	int framecount;
	float absoluteframetime;
	float curtime;
	float frametime;
	int maxclients;
	int tickcount;
	float interval_per_tick;
	float interpolation_amount;
};
//===================================================================================================
class CUserCmd
{
public:
	CUserCmd()
	{
		command_number = 0;
		tick_count = 0;
		viewangles.Init();
		forwardmove	  = 0.0f;
		sidemove		 = 0.0f;
		upmove		   = 0.0f;
		buttons		  = 0;
		impulse		  = 0;
		weaponselect	 = 0;
		weaponsubtype	= 0;
		random_seed	  = 0;
		mousedx		  = 0;
		mousedy		  = 0;
		hasbeenpredicted = false;
	}

	void copyFrom(const CUserCmd &src)
	{
		//command_number = src.command_number;
		tick_count = src.tick_count;
		viewangles = src.viewangles;
		forwardmove = src.forwardmove;
		sidemove = src.sidemove;
		upmove = src.upmove;
		buttons = src.buttons;
		impulse = src.impulse;
		weaponselect = src.weaponselect;
		weaponsubtype = src.weaponsubtype;
		//random_seed = src.random_seed;
		mousedx = src.mousedx;
		mousedy = src.mousedy;

		hasbeenpredicted = src.hasbeenpredicted;
	}

	virtual ~CUserCmd(){}; // Destructor 0
	int command_number;	// 4
	int tick_count;		   // 8
	Vector viewangles;	 // C
	float forwardmove;	 // 18
	float sidemove;		   // 1C
	float upmove;		   // 20
	int buttons;		   // 24
	BYTE impulse;		   // 28
	int weaponselect;	  // 2C
	int weaponsubtype;	 // 30
	int random_seed;	   // 34
	short mousedx;		   // 38
	short mousedy;		   // 3A
	bool hasbeenpredicted; // 3C;
};
//===================================================================================================
class CVerifiedUserCmd
{
public:
	CUserCmd m_cmd;
	CRC32_t m_crc;
};
//===================================================================================================
class CModelInfo
{
public:
	const char *GetModelName(DWORD *model)
	{
		typedef const char *(__thiscall * OriginalFn)(PVOID, DWORD *);
		return getvfunc<OriginalFn>(this, 3)(this, model);
	}
	DWORD *GetStudiomodel(DWORD *model)
	{
		typedef DWORD *(__thiscall * OriginalFn)(PVOID, DWORD *);
		return getvfunc<OriginalFn>(this, 28)(this, model);
	}
	int GetModelMaterialCount(model_t *model)
	{
		typedef int(__thiscall *OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 15)(this);
	}
	void GetModelMaterials(model_t *model, int count, IMaterial **ppMaterials)
	{
		typedef void(__thiscall *OriginalFn)(PVOID, model_t *, int, IMaterial **);
		return getvfunc<OriginalFn>(this, 16)(this, model, count, ppMaterials);
	}
};
//===================================================================================================
struct mstudiobbox_t
{
	int bone;
	int group;
	Vector bbmin;
	Vector bbmax;
	int szhitboxnameindex;
	int unused[8];
};
//===================================================================================================
struct mstudiohitboxset_t
{
	int sznameindex;
	inline char *const pszName(void) const { return ((char *)this) + sznameindex; }
	int numhitboxes;
	int hitboxindex;
	mstudiobbox_t *pHitbox(int i) const { return (mstudiobbox_t *)(((BYTE *)this) + hitboxindex) + i; };
};
//===================================================================================================
class ICollideable
{
public:
	// Gets at the entity handle associated with the collideable
	virtual IHandleEntity	*GetEntityHandle() = 0;

	// These methods return the bounds of an OBB measured in "collision" space
	// which can be retreived through the CollisionToWorldTransform or
	// GetCollisionOrigin/GetCollisionAngles methods
	virtual const Vector&	OBBMinsPreScaled() const = 0;
	virtual const Vector&	OBBMaxsPreScaled() const = 0;
	virtual const Vector&	OBBMins() const = 0;
	virtual const Vector&	OBBMaxs() const = 0;

	// Returns the bounds of a world-space box used when the collideable is being traced
	// against as a trigger. It's only valid to call these methods if the solid flags
	// have the FSOLID_USE_TRIGGER_BOUNDS flag set.
	virtual void			WorldSpaceTriggerBounds(Vector *pVecWorldMins, Vector *pVecWorldMaxs) const = 0;

	// custom collision test
	virtual bool			TestCollision(const Ray_t &ray, unsigned int fContentsMask, trace_t& tr) = 0;

	// Perform hitbox test, returns true *if hitboxes were tested at all*!!
	virtual bool			TestHitboxes(const Ray_t &ray, unsigned int fContentsMask, trace_t& tr) = 0;

	// Returns the BRUSH model index if this is a brush model. Otherwise, returns -1.
	virtual int				GetCollisionModelIndex() = 0;

	// Return the model, if it's a studio model.
	virtual const model_t*	GetCollisionModel() = 0;

	// Get angles and origin.
	virtual const Vector&	GetCollisionOrigin() const = 0;
	virtual const Vector&	GetCollisionAngles() const = 0;
	virtual const matrix3x4&	CollisionToWorldTransform() const = 0;

	// Return a SOLID_ define.
	virtual /*SolidType_t*/int GetSolid() const = 0;
	virtual int				GetSolidFlags() const = 0;

	// Gets at the containing class...
	virtual CBaseEntity*	GetIClientUnknown() = 0;

	// We can filter out collisions based on collision group
	virtual int				GetCollisionGroup() const = 0;

	// Returns a world-aligned box guaranteed to surround *everything* in the collision representation
	// Note that this will surround hitboxes, trigger bounds, physics.
	// It may or may not be a tight-fitting box and its volume may suddenly change
	virtual void			WorldSpaceSurroundingBounds(Vector *pVecMins, Vector *pVecMaxs) = 0;

	virtual bool			ShouldTouchTrigger(int triggerSolidFlags) const = 0;

	// returns NULL unless this collideable has specified FSOLID_ROOT_PARENT_ALIGNED
	virtual const matrix3x4	*GetRootParentToWorldTransform() const = 0;
};
//===================================================================================================
class CBaseEntity
{
public:
	CBaseHandle GetRefEHandle()
	{
		typedef CBaseHandle(__thiscall *OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 1)(this);
	}
	ICollideable *GetCollideable()
	{
		typedef ICollideable *(__thiscall *OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 2)(this);
	}
	Vector &GetAbsOrigin()
	{
		typedef Vector &(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 9)(this);
	}
	Vector &GetAbsAngles()
	{
		typedef Vector &(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 10)(this);
	}
	void GetWorldSpaceCenter(Vector &vWorldSpaceCenter)
	{
		Vector vMin, vMax;
		this->GetRenderBounds(vMin, vMax);
		vWorldSpaceCenter = this->GetAbsOrigin();
		vWorldSpaceCenter.z += (vMin.z + vMax.z) / 2;
	}
	bool IsBaseCombatWeapon()
	{
		typedef bool(__thiscall *OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 137)(this);
	}
	bool Interpolate( float currentTime )
	{
		typedef bool( __thiscall *OriginalFn )( PVOID, float );
		return getvfunc<OriginalFn>( this, 97 )( this, currentTime );
	}
	DWORD *GetModel()
	{
		PVOID pRenderable = static_cast<PVOID>(this + 0x4);
		typedef DWORD *(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(pRenderable, 9)(pRenderable);
	}
	bool SetupBones(matrix3x4 *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
	{
		PVOID pRenderable = static_cast<PVOID>(this + 0x4);
		typedef bool(__thiscall * OriginalFn)(PVOID, matrix3x4 *, int, int, float);
		return getvfunc<OriginalFn>(pRenderable, 16)(pRenderable, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
	}
	ClientClass *GetClientClass()
	{
		PVOID pNetworkable = static_cast<PVOID>(this + 0x8);
		typedef ClientClass *(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(pNetworkable, 2)(pNetworkable);
	}
	bool IsDormant()
	{
		PVOID pNetworkable = static_cast<PVOID>(this + 0x8);
		typedef bool(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(pNetworkable, 8)(pNetworkable);
	}
	int GetIndex()
	{
		PVOID pNetworkable = static_cast<PVOID>(this + 0x8);
		typedef int(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(pNetworkable, 9)(pNetworkable);
	}
	void GetRenderBounds(Vector &mins, Vector &maxs)
	{
		PVOID pRenderable = static_cast<PVOID>(this + 0x4);
		typedef void(__thiscall * OriginalFn)(PVOID, Vector &, Vector &);
		getvfunc<OriginalFn>(pRenderable, 20)(pRenderable, mins, maxs);
	}
	Vector &GetPrevLocalAngles()
	{
		typedef Vector&(__thiscall *OriginalFn)(PVOID);
		// could be 66
		return getvfunc<OriginalFn>(this, 142)(this);
	}
};
//===================================================================================================
class EngineClient
{
public:

	void GetScreenSize(int &width, int &height)
	{
		// call the engine function
		typedef void(__thiscall * OriginalFn)(PVOID, int &, int &);
		getvfunc<OriginalFn>(this, 5)(this, width, height);

		// if we are fullscreen these will be 0
		//if(width == 0 || height == 0)
		//{
		//	RECT size;

		//	memset( &size, 0, sizeof( size ) );

		//	HWND tfWindow;

		//	while( ( tfWindow = FindWindow( "Valve001", NULL ) ) == false ) Sleep( 200 );

		//	if( tfWindow == NULL )
		//	{
		//		Log::Fatal( "We have failed." );
		//	}

		//	GetWindowRect( tfWindow, &size);

		//	width  = size.right - size.left;
		//	height = size.bottom - size.top;

		//	Log::Console("Right: %d, Left: %d", size.right, size.left);
		//	Log::Console("Bottom: %d, Top: %d", size.bottom, size.top);

		//	Log::Console("Height: %d, width: %d", height, width);
		//}
	}
	bool GetPlayerInfo(int ent_num, player_info_t *pinfo)
	{
		typedef bool(__thiscall * OriginalFn)(PVOID, int, player_info_t *);
		return getvfunc<OriginalFn>(this, 8)(this, ent_num, pinfo);
	}
	player_info_t GetPlayerInfo(int ent_num)
	{
		player_info_t tempInfo;
		GetPlayerInfo(ent_num, &tempInfo);
		return tempInfo;
	}

	bool Con_IsVisible(void)
	{
		typedef bool(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 11)(this);
	}
	int GetLocalPlayer(void)
	{
		typedef int(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 12)(this);
	}
	float Time(void)
	{
		typedef float(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 14)(this);
	}
	void GetViewAngles(Vector &va)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, Vector &);
		return getvfunc<OriginalFn>(this, 19)(this, va);
	}
	void SetViewAngles(Vector &va)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, Vector &);
		return getvfunc<OriginalFn>(this, 20)(this, va);
	}
	int GetMaxClients(void)
	{
		typedef int(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 21)(this);
	}
	bool IsInGame(void)
	{
		typedef bool(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 26)(this);
	}
	bool IsConnected(void)
	{
		typedef bool(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 27)(this);
	}
	bool IsDrawingLoadingImage(void)
	{
		typedef bool(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 28)(this);
	}
	const matrix3x4 &WorldToScreenMatrix(void)
	{
		typedef const matrix3x4 &(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 36)(this);
	}
	bool IsTakingScreenshot(void)
	{
		typedef bool(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 85)(this);
	}
	CNetChan *GetNetChannelInfo(void)
	{
		typedef CNetChan *(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 72)(this);
	}
	void ExecuteClientCmd(const char *chCommandString)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, const char *);
		return getvfunc<OriginalFn>(this, 102)(this, chCommandString);
	}
	void ClientCmd_Unrestricted(const char *chCommandString)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, const char *);
		return getvfunc<OriginalFn>(this, 106)(this, chCommandString);
	}
	void ServerCmdKeyValues(PVOID keyval)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, PVOID);
		return getvfunc<OriginalFn>(this, 127)(this, keyval);
	}
	void ServerCmdKeyValues(KeyValues *keyval)
	{
		// call the other one
		return ServerCmdKeyValues((PVOID)keyval);
	}
};
//===================================================================================================
class IPanel
{
public:
	const char *GetName(unsigned int vguiPanel)
	{
		typedef const char *(__thiscall * OriginalFn)(PVOID, unsigned int);
		return getvfunc<OriginalFn>(this, 36)(this, vguiPanel);
	}
};
//===================================================================================================
class ISurface
{
public:
	void DrawSetColor(int r, int g, int b, int a)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, int, int, int, int);
		getvfunc<OriginalFn>(this, 11)(this, r, g, b, a);
	}
	void DrawFilledRect(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, int, int, int, int);
		getvfunc<OriginalFn>(this, 12)(this, x0, y0, x1, y1);
	}
	void DrawOutlinedRect(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, int, int, int, int);
		getvfunc<OriginalFn>(this, 14)(this, x0, y0, x1, y1);
	}
	void DrawOutlinedCircle(int x, int y, int radius, int segments)
	{
		typedef void(__thiscall *OriginalFn)(PVOID, int, int, int, int);
		return getvfunc<OriginalFn>(this, 99)(this, x, y, radius, segments);
	}
	void DrawLine(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, int, int, int, int);
		getvfunc<OriginalFn>(this, 15)(this, x0, y0, x1, y1);
	}
	void DrawPolyLine(int *x, int *y, int n)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, int *, int *, int);
		getvfunc<OriginalFn>(this, 16)(this, x, y, n);
	}

	void DrawSetTextFont(unsigned long font)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, unsigned long);
		getvfunc<OriginalFn>(this, 17)(this, font);
	}
	void DrawSetTextColor(int r, int g, int b, int a)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, int, int, int, int);
		getvfunc<OriginalFn>(this, 19)(this, r, g, b, a);
	}
	void DrawSetTextPos(int x, int y)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, int, int);
		getvfunc<OriginalFn>(this, 20)(this, x, y);
	}
	void DrawPrintText(const wchar_t *text, int textLen)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, const wchar_t *, int, int);
		return getvfunc<OriginalFn>(this, 22)(this, text, textLen, 0);
	}
	unsigned long CreateFont()
	{
		typedef unsigned int(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 66)(this);
	}
	void SetFontGlyphSet(unsigned long &font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, unsigned long, const char *, int, int, int, int, int, int, int);
		getvfunc<OriginalFn>(this, 67)(this, font, windowsFontName, tall, weight, blur, scanlines, flags, 0, 0);
	}
	void GetTextSize(unsigned long font, const wchar_t *text, int &wide, int &tall)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, unsigned long, const wchar_t *, int &, int &);
		getvfunc<OriginalFn>(this, 75)(this, font, text, wide, tall);
	}

	void PlaySound(const char *sound)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, const char *);
		return getvfunc<OriginalFn>(this, 78)(this, sound);
	}
	void UnlockCursor()
	{
		return getvfunc<void( __thiscall * )( PVOID )>( this, 61 )( this );
	}
	void LockCursor()
	{
		return getvfunc<void( __thiscall * )( PVOID )>( this, 62 )( this );
	}
	bool IsCursorLocked()
	{
		return getvfunc<bool( __thiscall * )( PVOID )>( this, 104 )( this );
	}
	void SetCursorAlwaysVisible( bool vis )
	{
		return getvfunc<void( __thiscall * )( PVOID, bool )>( this, 52 )( this, vis );
	}
	void SurfaceGetCursorPos( int &x, int &y )
	{
		return getvfunc<void( __thiscall * )( PVOID, int &, int & )>( this, 96 )( this, x, y );
	}
};
//===================================================================================================
class CEntList
{
public:
	CBaseEntity *GetClientEntity(int entnum)
	{
		typedef CBaseEntity *(__thiscall * OriginalFn)(PVOID, int);
		return getvfunc<OriginalFn>(this, 3)(this, entnum);
	}
	CBaseEntity *GetClientEntityFromHandle(CBaseHandle hEnt)
	{
		typedef CBaseEntity *(__thiscall * OriginalFn)(PVOID, CBaseHandle);
		return getvfunc<OriginalFn>(this, 4)(this, hEnt);
	}
	int GetHighestEntityIndex(void)
	{
		typedef int(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 6)(this);
	}
};
//===================================================================================================
class IPhysicsSurfaceProps
{
public:
	virtual ~IPhysicsSurfaceProps(void) {}

	// parses a text file containing surface prop keys
	virtual int		ParseSurfaceData(const char *pFilename, const char *pTextfile) = 0;
	// current number of entries in the database
	virtual int		SurfacePropCount(void) const = 0;

	virtual int		GetSurfaceIndex(const char *pSurfacePropName) const = 0;
	virtual void	GetPhysicsProperties(int surfaceDataIndex, float *density, float *thickness, float *friction, float *elasticity) const = 0;

	virtual /*surfacedata_t*/ void *GetSurfaceData(int surfaceDataIndex) = 0;
	virtual const char		*GetString(unsigned short stringTableIndex) const = 0;


	virtual const char		*GetPropName(int surfaceDataIndex) const = 0;

	// sets the global index table for world materials
	virtual void	SetWorldMaterialIndexTable(int *pMapArray, int mapSize) = 0;

	// NOTE: Same as GetPhysicsProperties, but maybe more convenient
	virtual void	GetPhysicsParameters(int surfaceDataIndex, /*surfacephysicsparams_t*/ void *pParamsOut) const = 0;
};
//===================================================================================================
class CBaseCombatWeapon
{
public:
	int GetMaxClip1()
	{
		typedef int(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 318)(this);
	}

	int GetMaxClip2()
	{
		typedef int(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 319)(this);
	}

	int GetSlot()
	{
		typedef int(__thiscall *OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 327)(this);
	}

	char *GetName()
	{
		typedef char *(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 329)(this);
	}

	char *GetPrintName()
	{
		typedef char *(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 330)(this);
	}

	Vector &GetBulletSpread()
	{
		typedef Vector &(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 286)(this);
	}
};
//===================================================================================================
class CTFBaseWeapon
{
public:
	// TODO impl nessacary functions here

	int getWeaponID()
	{
		typedef int(__thiscall *OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 422)(this);
	}
};
//===================================================================================================
class CTFBaseWeaponGun
{
public:
	float WeaponGetSpread()
	{
		typedef float(__thiscall * OriginalFn)(PVOID);
		// ida confirms 453
		return getvfunc<OriginalFn>(this, 453)(this);
	}
};
//===================================================================================================
class CBaseCombatCharacter
{
public:
	// tested
	void UpdateGlowEffect()
	{
		typedef void(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 226)(this);
	}
	void DestroyGlowEffect()
	{
		typedef void(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 227)(this);
	}
};
//===================================================================================================
class CTFBaseWeaponMelee
{
public:
	bool DoSwingTrace(trace_t &trace)
	{
		typedef bool(__thiscall * OriginalFn)(PVOID, trace_t &);
		return getvfunc<OriginalFn>(this, 443)(this, trace);
	}

	// unused
private:
	bool CalcIsAttackCriticalHelper()
	{
		typedef bool(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 389)(this);
	}
};
//===================================================================================================
abstract_class IAppSystem
{
public:
	// Here's where the app systems get to learn about each other 
	virtual bool Connect(CreateInterfaceFn factory) = 0;
	virtual void Disconnect() = 0;

	// Here's where systems can access other interfaces implemented by this object
	// Returns NULL if it doesn't implement the requested interface
	virtual void *QueryInterface(const char *pInterfaceName) = 0;

	// Init, shutdown
	virtual int Init() = 0;
	virtual void Shutdown() = 0;
};
//===================================================================================================
//class ICvar
//{
//public:
//	void ConsoleColorPrintf(const Color &clr, const char *fmt, ...)
//	{
//		// we cant simply pass on varargs to other functions, so we have to set up a va_list and pass that
//		char buf[950]; // max is 989: https://developer.valvesoftware.com/wiki/Developer_Console_Control
//		va_list vlist;
//		va_start(vlist, fmt);
//		_vsnprintf(buf, sizeof(buf), fmt, vlist);
//		va_end(vlist);
//		typedef void(__cdecl * OriginalFn)(PVOID, const Color &, const char *, va_list);
//		getvfunc<OriginalFn>(this, 23)(this, clr, "%s", buf);
//	}
//
//	void ConsolePrintf(const char *fmt, ...)
//	{
//		// we cant simply pass on varargs to other functions, so we have to set up a va_list and pass that
//		char buf[950]; // max is 989: https://developer.valvesoftware.com/wiki/Developer_Console_Control
//		va_list vlist;
//		va_start(vlist, fmt);
//		_vsnprintf(buf, sizeof(buf), fmt, vlist);
//		va_end(vlist);
//		typedef void(__cdecl * OriginalFn)(PVOID, const char *, va_list);
//		getvfunc<OriginalFn>(this, 24)(this, "%s", buf);
//	}
//
//	// find a var by name
//	ConVar *FindVar(const char *pszName)
//	{
//		typedef ConVar *(__thiscall * OriginalFn)(PVOID, const char *);
//		return getvfunc<OriginalFn>(this, 12)(this, pszName);
//	}
//
//	// get the starting command in the chain
//	ConCommandBase *GetCommands()
//	{
//		typedef ConCommandBase *(__thiscall * OriginalFn)(PVOID);
//		return getvfunc<OriginalFn>(this, 16)(this);
//	}
//
//	// register a new ConCommand
//	void RegisterConCommand(ConCommandBase *base)
//	{
//		typedef void(__thiscall * OriginalFn)(PVOID, ConCommandBase *);
//		return getvfunc<OriginalFn>(this, 6)(this, base);
//	}
//
//	int AllocateDLLIdentifier()
//	{
//		typedef int(__thiscall *OriginalFn)(PVOID);
//		return getvfunc<OriginalFn>(this, 5)(this);
//	}
//};

abstract_class ICvar : public IAppSystem
{
public:
	// Allocate a unique DLL identifier
	virtual int AllocateDLLIdentifier() = 0;

	// Register, unregister commands
	virtual void			RegisterConCommand(ConCommandBase *pCommandBase) = 0;
	virtual void			UnregisterConCommand(ConCommandBase *pCommandBase) = 0;
	virtual void			UnregisterConCommands(int id) = 0;

	// If there is a +<varname> <value> on the command line, this returns the value.
	// Otherwise, it returns NULL.
	virtual const char*		GetCommandLineValue(const char *pVariableName) = 0;

	// Try to find the cvar pointer by name
	virtual ConCommandBase *FindCommandBase(const char *name) = 0;
	virtual const ConCommandBase *FindCommandBase(const char *name) const = 0;
	virtual ConVar			*FindVar(const char *var_name) = 0;
	virtual const ConVar	*FindVar(const char *var_name) const = 0;
	virtual ConCommand		*FindCommand(const char *name) = 0;
	virtual const ConCommand *FindCommand(const char *name) const = 0;

	// Get first ConCommandBase to allow iteration
	virtual ConCommandBase	*GetCommands(void) = 0;
	virtual const ConCommandBase *GetCommands(void) const = 0;

	// Install a global change callback (to be called when any convar changes) 
	virtual void			InstallGlobalChangeCallback(FnChangeCallback_t callback) = 0;
	virtual void			RemoveGlobalChangeCallback(FnChangeCallback_t callback) = 0;
	virtual void			CallGlobalChangeCallbacks(ConVar *var, const char *pOldString, float flOldValue) = 0;

	// Install a console printer
	virtual void			InstallConsoleDisplayFunc(void* pDisplayFunc) = 0;
	virtual void			RemoveConsoleDisplayFunc(void* pDisplayFunc) = 0;
	virtual void			ConsoleColorPrintf(const Color& clr,  const char *pFormat, ...) const = 0;
	virtual void			ConsolePrintf(const char *pFormat, ...) const = 0;
	virtual void			ConsoleDPrintf(const char *pFormat, ...) const = 0;

	// Reverts cvars which contain a specific flag
	virtual void			RevertFlaggedConVars(int nFlag) = 0;

	// Method allowing the engine ICvarQuery interface to take over
	// A little hacky, owing to the fact the engine is loaded
	// well after ICVar, so we can't use the standard connect pattern
	virtual void			InstallCVarQuery(void *pQuery) = 0;

	#if defined( _X360 )
	virtual void			PublishToVXConsole() = 0;
	#endif
	virtual bool			IsMaterialThreadSetAllowed() const = 0;
	virtual void			QueueMaterialThreadSetValue(ConVar *pConVar, const char *pValue) = 0;
	virtual void			QueueMaterialThreadSetValue(ConVar *pConVar, int nValue) = 0;
	virtual void			QueueMaterialThreadSetValue(ConVar *pConVar, float flValue) = 0;
	virtual bool			HasQueuedMaterialThreadConVarSets() const = 0;
	virtual int				ProcessQueuedMaterialThreadConVarSets() = 0;

protected:	class ICVarIteratorInternal;
public:
	/// Iteration over all cvars. 
	/// (THIS IS A SLOW OPERATION AND YOU SHOULD AVOID IT.)
	/// usage: 
	/// { ICVar::Iterator iter(g_pCVar); 
	///   for ( iter.SetFirst() ; iter.IsValid() ; iter.Next() )
	///   {  
	///       ConCommandBase *cmd = iter.Get();
	///   } 
	/// }
	/// The Iterator class actually wraps the internal factory methods
	/// so you don't need to worry about new/delete -- scope takes care
	//  of it.
	/// We need an iterator like this because we can't simply return a 
	/// pointer to the internal data type that contains the cvars -- 
	/// it's a custom, protected class with unusual semantics and is
	/// prone to change.
	class Iterator
	{
	public:
		inline Iterator(ICvar *icvar);
		inline ~Iterator(void);
		inline void		SetFirst(void);
		inline void		Next(void);
		inline bool		IsValid(void);
		inline ConCommandBase *Get(void);
	private:
		ICVarIteratorInternal *m_pIter;
	};

protected:
	// internals for  ICVarIterator
	class ICVarIteratorInternal
	{
	public:
		// warning: delete called on 'ICvar::ICVarIteratorInternal' that is abstract but has non-virtual destructor [-Wdelete-non-virtual-dtor]
		virtual ~ICVarIteratorInternal() {}
		virtual void		SetFirst(void) = 0;
		virtual void		Next(void) = 0;
		virtual	bool		IsValid(void) = 0;
		virtual ConCommandBase *Get(void) = 0;
	};

	virtual ICVarIteratorInternal	*FactoryInternalIterator(void) = 0;
	friend class Iterator;
};
//===================================================================================================
class ClientModeShared
{
public:
	bool IsChatPanelOutOfFocus(void)
	{
		typedef PVOID(__thiscall * OriginalFn)(PVOID);
		PVOID CHudChat = getvfunc<OriginalFn>(this, 19)(this);
		if(CHudChat)
		{
			return *(PFLOAT)((DWORD)CHudChat + 0xFC) == 0;
		}
		return false;
	}
};
//===================================================================================================
class CInput
{
public:
	// get the current cmd
	CUserCmd *GetUserCmd(int seq)
	{
		// call the original one, which will  then call the hooked one
		typedef CUserCmd *(__thiscall * OriginalFn)(PVOID, int);
		return getvfunc<OriginalFn>(this, 8)(this, seq);
	}
};
//===================================================================================================
class CSteamInterfaces
{
public:
	// ISteamClient017 	*client;
	nn_interface<ISteamClient017 *> client;

	// ISteamFriends002 	*friends;
	nn_interface<ISteamFriends002 *> friends;

	// ISteamUser017 	*user;
	nn_interface<ISteamUser017 *> user;

	// ISteamUserStats011 *userStats;
	nn_interface<ISteamUserStats011 *> userStats;
};
//===================================================================================================
class CInterfaces
{
public:
	// CEntList 				*EntList;
	nn_interface<CEntList *> EntList;

	// EngineClient 			*Engine;
	nn_interface<EngineClient *> Engine;

	// IPanel 			*Panels;
	nn_interface<IPanel *> Panels;

	// ISurface 				*Surface;
	nn_interface<ISurface *> Surface;

	// ClientModeShared 		*ClientMode;
	nn_interface<ClientModeShared *> ClientMode;

	// CHLClient 				*Client;
	nn_interface<CHLClient *> Client;

	// ICvar 					*Cvar;
	nn_interface<ICvar *> Cvar;

	// CEngineTrace 			*EngineTrace;
	nn_interface<CEngineTrace *> EngineTrace;

	// CModelInfo 				*ModelInfo;
	nn_interface<CModelInfo *> ModelInfo;

	// CInput 					*Input;
	nn_interface<CInput *> Input;

	CSteamInterfaces steam;

	// CGlobals 				*Globals;
	nn_interface<CGlobals *> Globals;

	// CUniformRandomStream 	*RandomStream;
	nn_interface<CUniformRandomStream *> RandomStream;

	// CPrediction 			*Prediction;
	nn_interface<CPrediction *> Prediction;

	// IMoveHelper 			*MoveHelper;
	nn_interface<IMoveHelper *> MoveHelper;

	// IEngineSound			*SoundEngine;
	nn_interface<IEngineSound *> SoundEngine;

	// CPlayerResource			*GameResource;
	nn_interface<CPlayerResource *> GameResource;

	// CBaseClientState		*ClientState;
	nn_interface<CClientState *> ClientState;

	nn_interface<CDemoPlayer *> DemoPlayer;

	nn_interface<IGameEventManager2 *> EventManager;

	nn_interface<PVOID> FileSystem;

	nn_interface<IGameMovement *> GameMovement;

	nn_interface<CMoveData *> MoveData;

	nn_interface<IVDebugOverlay *> DebugOverlay;

	nn_interface<IPhysicsSurfaceProps *> PhysicsSurfaceProps;

	nn_interface<CModelRender *> ModelRender;

	nn_interface<CMaterialSystem *> MatSystem;

	nn_interface<CRenderView *> RenderView;

	HWND thisWindow;
	WNDPROC oldWindowProc;
	HMODULE thisDll;
};
//===================================================================================================
typedef struct predictedVars_s
{
	Vector origin;
	Vector oldOrigin;
} predictedVars_t;
//===================================================================================================
typedef struct playerVars_s
{
	tf_classes Class = tf_classes::TF2_Invalid;
	int cond;
	int condEx;
	int health;
	int team;
	int flags;
	classId activeWeapon;
	int cmdNum;
	float flNextAttack;
	std::string name; // update this when we change it - gets set every tick
	player_info_t info;
	predictedVars_t pred;
	Vector viewOffset;
	CEntTag wepTag;
} playerVars_t;
//===================================================================================================
inline IHandleEntity *CBaseHandle::Get()
{
	extern CInterfaces gInts;
	return (IHandleEntity *)gInts.EntList->GetClientEntityFromHandle(*this);
}
//===================================================================================================
extern CInterfaces gInts;
extern CPlayerVariables gEntVars;
extern COffsets gOffsets;
extern playerVars_t gLocalPlayerVars;

class CHack;
extern CHack gHack;

class CFileManager;
extern CFileManager gFileManager;
//===================================================================================================
#include "CFileManager.h"
#include "CHack.h"
