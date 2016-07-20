#pragma once

#include <vector>
#include "IHack.h"

enum class ButtonCode_t;

class F1_MenuListing : public F1_CComponent
{
	F1_IConVar *pVar;
	int xx;
	DWORD dwColor;

protected:

	using BaseClass = F1_CComponent;

public:

	F1_MenuListing( int x, int y, int w, int h, F1_IConVar *p, int xx, DWORD dwColor ) : F1_CComponent( x, y, w, h ), pVar( p ), xx( xx ), dwColor( dwColor )
	{}

	void render() override;

	bool handleMouseInput( int x, int y, mouseButton m ) override;
};

class F1_Menu : public F1_CWindow
{
	F1_IConVar *menuArray[ 250 ];
	int menuIndex = 0;
	bool menuEnabled = false;

	int lastHeight;

protected:

	using BaseClass = F1_CWindow;

public:

	F1_Menu( int x, int y, int w );

	virtual F1_Point getWidthHeight() override;

	void render() override;

	int keyboardInput( ButtonCode_t button );
};
