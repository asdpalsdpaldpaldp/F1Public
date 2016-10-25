#include "SDK.h"
#include "CDrawManager.h"
#include "Util.h"

#include "F1_Menu.h"

#include "modules.h"

DEFINE_RECURSE_CALL_FUNCTION_2_ARG( _menuUpdate, F1_IConVar **, int & );

void F1_MenuListing::render()
{
	F1_Point mouseXY;
	gInts.Surface->SurfaceGetCursorPos( mouseXY.x, mouseXY.y );

	F1_Point xy = getPos();

	F1_Point wh = getWidthHeight();

	DWORD dwColor = COLORCODE( 255, 255, 255, 255 );
	DWORD dwTeamColor = dwColor;

	if( CUtil::isPointWithinRange( mouseXY, getPos(), getWidthHeight() ) )
	{
		// hwighlight
		gDrawManager.DrawRect( xy.x + 1, xy.y, wh.x - 2, wh.y, COLORCODE( 255, 255, 255, 80 ) );
		dwColor = dwTeamColor = gDrawManager.dwGetTeamColor( CEntity<>{me}.get<int>( gEntVars.iTeam ) );
	}
	else
	{
		// parent window draws normal background
		dwTeamColor = gDrawManager.dwGetTeamColor( CEntity<>{me}.get<int>( gEntVars.iTeam ) );
	}

	// use of dynamic_cast here allows us to up cast from the base class to the parent class
	// without worrying about what type it is
	// (if pVar is not a F1_ConVar<Switch> * internally then the dynamic cast will return nullptr)
	if( auto *pSwitchConvar = dynamic_cast< F1_ConVar<Switch> * >( pVar ) )
	{
		if( pSwitchConvar->getValue() )
			gDrawManager.DrawString( "menuMain", xy.x + 2, xy.y, dwTeamColor, " [-] %s", pSwitchConvar->name() );
		else
			gDrawManager.DrawString( "menuMain", xy.x + 2, xy.y, dwTeamColor, " [+] %s", pSwitchConvar->name() );
	}
	else
	{
		gDrawManager.DrawString( "menuMain", xy.x + 2, xy.y, dwColor, "%s", pVar->name() );
		gDrawManager.DrawString( "menuMain", xx, xy.y, dwColor, "%s", pVar->print() );
	}

	BaseClass::render();
}

bool F1_MenuListing::handleMouseInput( int x, int y, mouseButton m )
{
	//Log::Console("Handle input menuListing");

	if( CUtil::isPointWithinRange( { x,y }, getPos(), getWidthHeight() ) )
	{
		//Log::Console("WithinRange");

		if( auto *pSwitchConvar = dynamic_cast< F1_ConVar<Switch> * >( pVar ) )
		{
			// lup will only happen once, but ldown will happen many times
			if( m == mouseButton::LUP )
			{
				// flip
				pSwitchConvar->decrement();
			}
			else if( m == mouseButton::RUP )
			{
				pSwitchConvar->increment();
			}
		}
		else if( auto *pBoolConvar = dynamic_cast< F1_ConVar<bool> * >( pVar ) )
		{
			if( m == mouseButton::LUP )
			{
				// flip
				pBoolConvar->decrement();
			}
			else if( m == mouseButton::RUP )
			{
				pBoolConvar->increment();
			}
		}
		else
		{
			if( m == mouseButton::LUP )
			{
				//Log::Console("decrement");
				pVar->decrement();
			}
			else if( m == mouseButton::RUP )
			{
				//Log::Console("increment");
				pVar->increment();
			}

			if( m == mouseButton::SCROLLUP )
			{
				pVar->increment();
			}
			else if( m == mouseButton::SCROLLDOWN )
			{
				pVar->decrement();
			}
		}

		return true;
	}

	return false;
}

F1_Menu::F1_Menu( int x, int y, int w ) : F1_CWindow( x, y, w, 0, x, y - gDrawManager.GetESPHeight(), w, gDrawManager.GetESPHeight() ), lastHeight( 0 )
{
	// clear out the menuArray
	memset( menuArray, 0, sizeof( menuArray ) );
}

F1_Point F1_Menu::getWidthHeight()
{
	int w = BaseClass::getWidthHeight().x;
	int h = 0;

	//while(menuArray[++h] != NULL) {}

	//auto fontHeight = gDrawManager.GetESPHeight();

	return{ w,lastHeight };
}

void F1_Menu::render()
{
	if( menuEnabled )
	{
		//CUtil::setMouseState(0);
		CUtil::setCursorVisible( true );

		F1_Point xy = getPos();

		const int x = xy.x;
		const int y = xy.y;

		const int xx = x + 130;

		const int w = getWidthHeight().x;

		// TODO replace with font manager system
		const static int h = gDrawManager.GetHudHeight();

		int currIndex = 0;

		// TODO we need to do this to ensure that when switches close, the items are deleted
		// memset is a fast operation, so this should not be a problem
		memset( menuArray, 0, sizeof( menuArray ) );

		// remove current listing children and destroy them
		destroyChildren();

		RecurseCall_menuUpdate( menuArray, currIndex, ACTIVE_HACKS );

		int iMenuItems = currIndex;

		lastHeight = iMenuItems * h;

		const DWORD dwTeamColor = gDrawManager.dwGetTeamColor( CEntity<>{me}.get<int>( gEntVars.iTeam ) );
		const DWORD dwMenuOff = COLORCODE( 255, 255, 255, 255 );

		gDrawManager.DrawString( "menuMain", x + 2, y - 18, dwTeamColor, "F12016" );

		for( int i = 0; i < iMenuItems; i++ )
		{
			auto *pVar = menuArray[ i ];

			if( pVar != NULL )
			{
				if( i != menuIndex )
				{
					addChild( new F1_MenuListing( x, y + ( h * i ), w, h, pVar, xx, dwMenuOff ) );
				}
				else
				{
					addChild( new F1_MenuListing( x, y + ( h * i ), w, h, pVar, xx, dwTeamColor ) );
				}
			}
		}

		// call the base render
		BaseClass::render();
	}
	else
	{
		//CUtil::setMouseState(1);
		CUtil::setCursorVisible( false );
	}

	// TODO merge
	if( menuEnabled )
	{
		int x, y;

		gInts.Surface->SurfaceGetCursorPos( x, y );

		gDrawManager.DrawRect( x - 6, y - 6, 6, 6, gDrawManager.dwGetTeamColor( CEntity<>{me}.get<int>( gEntVars.iTeam ) ) );
	}
}

int F1_Menu::keyboardInput( ButtonCode_t button )
{
	if( button == ButtonCode_t::KEY_INSERT )
	{
		menuEnabled = !menuEnabled;
	}
	// do not use else here, if we press insert, we want the menu to appear
	// deprecated in favour of mouse 
	if( menuEnabled && false)
	{
		switch( button )
		{
		case ButtonCode_t::KEY_UP:
		case ButtonCode_t::MOUSE_WHEEL_UP:

			if( menuIndex > 0 )
				menuIndex--;
			else
			{
				while( menuArray[ ++menuIndex ] != NULL );
				menuIndex -= 1;
			}
			return 0;

			break;

		case ButtonCode_t::KEY_DOWN:
		case ButtonCode_t::MOUSE_WHEEL_DOWN:

			if( menuArray[ menuIndex + 1 ] != NULL )
				menuIndex++;
			else
				menuIndex = 0;
			return 0;
			break;

		case ButtonCode_t::KEY_LEFT:

			menuArray[ menuIndex ]->decrement();

			return 0;
			break;

		case ButtonCode_t::KEY_RIGHT:
		case ButtonCode_t::MOUSE_RIGHT:

			menuArray[ menuIndex ]->increment();

			return 0;
			break;

			// TODO fix this by using virtual key states when the mouse is not engaged
		case ButtonCode_t::MOUSE_LEFT:
		case ButtonCode_t::KEY_ENTER:

			return 0;
			break;

		default:
			break;
		}
	}

	return 1;
}
