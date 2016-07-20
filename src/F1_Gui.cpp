#include "F1_Gui.h"

#include "F1_Gui.h"

#include "SDK.h"
#include "CDrawManager.h"
#include "Util.h"

F1_CComponent::F1_Handle F1_CComponent::lastHandle = 0;

bool F1_CWindow::inMove = false;

void F1_CComponent::render()
{
	// we need to make sure to render our children
	for( auto &child : children )
	{
		child->render();
	}

	#ifdef F1_GUI_DEBUG

	// draw the current xywh box
	gDrawManager.OutlineRect( getPos().x, getPos().y, getWidthHeight().x, getWidthHeight().y, COLORCODE( 0, 0, 255, 255 ) );

	#endif
}

bool F1_CWindow::handleMouseInput( int x, int y, mouseButton m )
{
	if( BaseClass::handleMouseInput( x, y, m ) )
	{
		return true;
	}
	else
	{
		inMove = true;

		F1_Rect bounds = getBounds();

		if( CUtil::isPointWithinRange( { x,y }, { bounds.x, bounds.y }, { bounds.w, bounds.h } ) )
		{
			//Log::Console("within range");

			// the cursor is within our bounds, check to see whether our children can handle it
			{
				// our children could not handle the input, time to do our input
				if( m == mouseButton::LDOWN )
				{
					if( !isMoving )
					{
						isMoving = true;
						oldM = CUtil::getMousePos();
						oldP = getPos();

						oldBounds = getBounds();
					}
				}
				else if( m == mouseButton::LUP )
				{
					isMoving = false;
					inMove = false;
					return true;
				}

				return true;
			}
		}
	}

	return false;
}

void F1_CWindow::think()
{
	// call children's think as well
	BaseClass::think();

	if( isMoving )
	{
		// TODO replace with a gCursor when that happens

		gInts.Surface->SurfaceGetCursorPos( newM.x, newM.y );

		difM = newM - oldM;

		F1_Point curPos = oldP;

		setPos( { curPos.x + difM.x, curPos.y + difM.y } );

		// update bounds
		setBounds( { oldBounds.x + difM.x, oldBounds.y + difM.y, bounds.w, bounds.h } );
	}
}

void F1_CWindow::render()
{
	// get our positional info
	F1_Point xy = getPos();
	F1_Point wh = getWidthHeight();

	// draw team colored outline
	gDrawManager.OutlineRect( xy.x, xy.y, wh.x, wh.y, gDrawManager.dwGetTeamColor( CEntity<>{me}.get<int>( gEntVars.iTeam ) ) );

	// draw grey backdrop
	const DWORD dwBack = COLORCODE( 20, 20, 20, 128 );

	gDrawManager.DrawRect( xy.x, xy.y, wh.x, wh.y, dwBack );

	// draw this windows bounds

	// set the bounds with the derived classes bounds
	setBounds( getBounds() );

	#ifdef F1_GUI_DEBUG
	gDrawManager.OutlineRect( bounds.x, bounds.y, bounds.w, bounds.h, COLORCODE( 0, 255, 0, 255 ) );
	#endif

	// call childrens render last so they draw ontop of us
	BaseClass::render();
}