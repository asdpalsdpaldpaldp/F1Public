#include "SDK.h"
#include "Panels.h"
#include "CDrawManager.h"

#include "Util.h"

//===================================================================================
void __fastcall CHack::Hooked_PaintTraverse(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce)
{
	//return gHack.paintTraverse(pPanels, edx, vguiPanel, forceRepaint, allowForce);

	_TRY
	{
		_INSTALL_SEH_TRANSLATOR();

		gInts.Surface->SetCursorAlwaysVisible( CUtil::cursorVisible );

		VMTManager &hook = VMTManager::GetHook( pPanels );																								 //Get a pointer to the instance of your VMTManager with the function GetHook.
		hook.GetMethod<void( __thiscall * )( PVOID, unsigned int, bool, bool )>( gOffsets.paintTraverseOffset )( pPanels, vguiPanel, forceRepaint, allowForce ); //Call the original.

		static unsigned int vguiMatSystemTopPanel;

		if( vguiMatSystemTopPanel == 0 ) // check to see if we found the panel
		{
			const char *szName = gInts.Panels->GetName( vguiPanel );
			if( szName[ 0 ] == 'M' && szName[ 3 ] == 'S' ) //Look for MatSystemTopPanel without using slow operations like strcmp or strstr.
			{
				vguiMatSystemTopPanel = vguiPanel;
				// run our intro
				gHack.intro();
			}
		}

		if( vguiMatSystemTopPanel == vguiPanel ) //If we're on MatSystemTopPanel, call our drawing code.
		{

			gInts.DebugOverlay->ClearAllOverlays();

			if( gInts.Engine->IsDrawingLoadingImage() || !gInts.Engine->IsInGame() || !gInts.Engine->IsConnected() || gInts.Engine->Con_IsVisible() || ( ( GetAsyncKeyState( VK_F12 ) || gInts.Engine->IsTakingScreenshot() ) ) )
				return; //We don't want to draw at the menu.

						//This section will be called when the player is not at the menu game and can see the screen or not taking a screenshot.
			gDrawManager.DrawString( "hud", ( gScreenSize.iScreenWidth / 2 ) - 55, 200, gDrawManager.dwGetTeamColor( gLocalPlayerVars.team ), "Welcome to F1" ); //Remove this if you want.

																																								 // debug string stuff

			int debugTexty = 600;
			//gDrawManager.DrawString("hud", 0, y, COLOR_OBJ, "old: %f %f %f", gLocalPlayerVars.pred.oldOrigin[0], gLocalPlayerVars.pred.oldOrigin[1], gLocalPlayerVars.pred.oldOrigin[2]);
			//debugTexty += gDrawManager.GetHudHeight();
			//gDrawManager.DrawString("hud", 0, y, COLOR_OBJ, "new: %f %f %f", gLocalPlayerVars.pred.origin[0], gLocalPlayerVars.pred.origin[1], gLocalPlayerVars.pred.origin[2]);
			//debugTexty += gDrawManager.GetHudHeight();
			//gDrawManager.DrawString("hud", 0, y, COLOR_OBJ, "dif: %f %f %f", gLocalPlayerVars.pred.origin[0] - gLocalPlayerVars.pred.oldOrigin[0], gLocalPlayerVars.pred.origin[1] - gLocalPlayerVars.pred.oldOrigin[1], gLocalPlayerVars.pred.origin[2] - gLocalPlayerVars.pred.oldOrigin[2]);
			//debugTexty += gDrawManager.GetHudHeight();
			//gDrawManager.DrawString("hud", 0, y, COLOR_OBJ, "curtime: %f", gInts.Globals->curtime);
			//debugTexty += gDrawManager.GetHudHeight();
			//gDrawManager.DrawString("hud", 0, y, COLOR_OBJ, "flNextAttack: %f", gLocalPlayerVars.flNextAttack);
			//debugTexty += gDrawManager.GetHudHeight();
			//gDrawManager.DrawString("hud", 0, y, COLOR_OBJ, "diff: %f", gLocalPlayerVars.flNextAttack - gInts.Globals->curtime);
			//debugTexty += gDrawManager.GetHudHeight();

			int i = 0;
			auto *pHack = gHack.pHackArray[ i++ ];
			while( pHack != NULL )
			{
				pHack->paint();

				// this can probably be improved
				for( int j = 1; j < gInts.EntList->GetHighestEntityIndex(); j++ )
				{
					auto *pBaseEntity = gInts.EntList->GetClientEntity( j );

					// do not call for null entitys (theres no point)
					if( pBaseEntity != NULL && pBaseEntity->GetIndex() != me )
					{
						_TRY
						{
							pHack->processEntity( j );
						}
						_CATCH_SEH_REPORT_ERROR( pHack, processEntity() );
					}
				}

				pHack = gHack.pHackArray[ i++ ];
			}

			for( auto &window : gHack.windowArray )
			{
				// render each window
				window->render();

				// think each window
				window->think();
			}
		}
	}
	_CATCHMODULE
	{
		Log::Error( "%s", e.what() );
	}
	_CATCH
	{
		Log::Error( "other error in CHack::paintTraverse" );
	}
}