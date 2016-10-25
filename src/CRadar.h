#pragma once

#include "IHack.h"

#include "F1_Gui.h"

class CRadar : public IHack<CRadar>, public F1_CWindow
{
	F1_ConVar<Switch> *pRadarSwitch;

	F1_ConVar<bool> *pEnabledRadar;

	F1_ConVar<int> *pSize;
	F1_ConVar<float> *pRange;

	using BaseClass = F1_CWindow;

public:

	CRadar() : F1_CWindow( 150, 150, 130, 130, 150, 150, 260, 260 )
	{
		pRadarSwitch = new F1_ConVar<Switch>( "Radar", false );

		pEnabledRadar = new F1_ConVar<bool>( " - Enabled", false );

		pSize = new F1_ConVar<int>( " - Size", 130, 0, 200, 5 );
		pRange = new F1_ConVar<float>( " - Range", 2000, 0, 10000, 100 );
	}

	void render() override;

	void processEntity( CBaseEntity *pBaseEntity );

	F1_Point getWidthHeight() override;

	F1_Rect getBounds() override;

	void menuUpdate( F1_IConVar **pMenuArray, int &currIndex );
};

extern CRadar gRadar;