#pragma once

#include "IHack.h"

#include "CEntity.h"

class CESP : public IHack<CESP>
{

	//bool enabled;
	//bool renderBox;
	//bool renderGUID;
	//bool renderName;
	//bool renderHealth;
	//bool renderFriends;
	//bool renderIndex;
	//bool renderObjectID;
	//bool renderViewESP;
	//float viewESPLength;

	//var enabled_bool{"Enabled", &enabled};
	//var box_bool{"ESP Box", &renderBox};
	//var guid_bool{"GUID", &renderGUID};
	//var name_bool{"Name", &renderName};
	//var health_bool{"Health", &renderHealth};
	//var friend_bool{"Friends?", &renderFriends};
	//var index_bool{"Index", &renderIndex};
	//var id_bool{"Object ID", &renderObjectID};
	//var view_bool{"View ESP", &renderViewESP};
	//var view_float{"View Length", &viewESPLength, 100.0f, 5000.0f, 500.0f, 100.0f};

	F1_ConVar<Switch> *espSwitch;
	F1_ConVar<bool> *renderBox;
	F1_ConVar<bool> *renderBones;
	F1_ConVar<bool> *renderGUID;
	F1_ConVar<bool> *renderName;
	F1_ConVar<bool> *renderHealth;
	F1_ConVar<bool> *renderIndex;
	F1_ConVar<bool> *renderObjectID;
	F1_ConVar<bool> *renderViewESP;
	F1_ConVar<int> *viewESPLength;
	F1_ConVar<bool> *hitboxes;

	// creds to Roskonix for these nice esp boxes
	void DynamicBox( CBaseEntity *pBaseEntity, DWORD dwColor );

	void FrameHitbox( CBaseEntity *pBaseEntity, int iHitbox );

	void drawBoneEsp( CBaseEntity *pBaseEntity, DWORD color );

public:
	CESP()
	{
		espSwitch = new F1_ConVar<Switch>( "ESP", false );
		renderBox = new F1_ConVar<bool>( " - Box", false );
		renderBones = new F1_ConVar<bool>( " - Bones", false );
		renderGUID = new F1_ConVar<bool>( " - GUID", false );
		renderName = new F1_ConVar<bool>( " - Name", false );
		renderHealth = new F1_ConVar<bool>( " - Health", false );
		renderIndex = new F1_ConVar<bool>( " - Index", false );
		renderObjectID = new F1_ConVar<bool>( " - ObjectID", false );
		renderViewESP = new F1_ConVar<bool>( " - View", false );
		viewESPLength = new F1_ConVar<int>( " - View Length", 500, 100, 2000, 100 );
		hitboxes = new F1_ConVar<bool>( " - Show hitboxes", false );
	}

	const char *name() const;

	void processEntity( CBaseEntity *pBaseEntity );

	void menuUpdate( F1_IConVar **menuArray, int &currIndex );
};

extern CESP gEsp;