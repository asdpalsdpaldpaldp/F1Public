#include "Util.h"

int GetNumHitboxes(CBaseEntity *ent)
{
	auto *model = ent->GetModel();
	auto *pHeader = gInts.ModelInfo->GetStudiomodel(model);
	int HitboxSetIndex = *( int * ) ( ( DWORD ) pHeader + 0xB0 );
	mstudiohitboxset_t *pSet = ( mstudiohitboxset_t * ) ( ( ( PBYTE ) pHeader ) + HitboxSetIndex );
	return pSet->numhitboxes;
}

int CUtil::mouseState = 1;
bool CUtil::cursorVisible = false;
bool CUtil::softwareCursor = false;

bool CUtil::isPlayer( CBaseEntity *pBaseEntity )
{
	return false;
}

F1_Point CUtil::getMousePos()
{
	F1_Point xy{ 0,0 };
	gInts.Surface->SurfaceGetCursorPos( xy.x, xy.y );
	return xy;
}
