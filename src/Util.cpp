#include "Util.h"

int CUtil::mouseState = 1;
bool CUtil::cursorVisible = false;

F1_Point CUtil::getMousePos()
{
	F1_Point xy{ 0,0 };
	gInts.Surface->SurfaceGetCursorPos( xy.x, xy.y );
	return xy;
}