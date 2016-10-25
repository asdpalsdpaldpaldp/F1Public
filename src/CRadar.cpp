#include "CRadar.h"

#include "CDrawManager.h"
#include "CEntity.h"
#include "CPlayerManager.h"

CRadar gRadar;

void CRadar::render()
{
	if( pEnabledRadar->getValue() == false )
		return;

	CBaseEntity *pLocalEntity = GetBaseEntity( me );

	// render the back
	BaseClass::render();

	F1_Point xy = getPos();

	int iCenterX = xy.x + pSize->getValue();
	int iCenterY = xy.y + pSize->getValue();
	int iSize = pSize->getValue();
	DWORD dwColor = gDrawManager.dwGetTeamColor( pLocalEntity->GetTeam() );

	gDrawManager.OutlineRect( iCenterX - iSize - 1, iCenterY - iSize - 1, 2 * iSize + 2 + 1, 2 * iSize + 2 + 1, COLORCODE( 0, 0, 0, 255 ) );
	//gDrawManager.OutlineRect(iCenterX - iSize - 2, iCenterY - iSize - 2, 2 * iSize + 2 + 3, 2 * iSize + 2 + 3, dwColor);
	//gDrawManager.DrawRect(iCenterX - iSize, iCenterY - iSize, 2 * iSize + 2, 2 * iSize + 2, COLORCODE(30, 30, 30, 128));
	iSize -= 10;

	gDrawManager.DrawRect( iCenterX, iCenterY - iSize - 11, 1, 2 * iSize + 22, dwColor );
	gDrawManager.DrawRect( iCenterX - iSize - 11, iCenterY, 2 * iSize + 22, 1, dwColor );
}

void CRadar::processEntity( CBaseEntity *pBaseEntity )
{

	if( pBaseEntity->IsDormant() || pEnabledRadar->getValue() == false )
		return;

	// only draw for players
	if( pBaseEntity->GetClientClass()->iClassID == classId::CTFPlayer && pBaseEntity->IsAlive() )
	{
		//CEntity<> local{ me };
		CBaseEntity *pLocalEntity = GetBaseEntity( me );
		if( pLocalEntity == nullptr )
			return;

		DWORD dwColor = gPlayerManager.getColorForPlayer( pBaseEntity->GetIndex() );
		if(dwColor == -1 )
			dwColor = gDrawManager.dwGetTeamColor( pBaseEntity->GetTeam() );

		F1_Point xy = getPos();

		int iCenterX = xy.x + pSize->getValue();
		int iCenterY = xy.y + pSize->getValue();

		int iSize = pSize->getValue();

		float flDeltaX = pBaseEntity->GetAbsOrigin().x - pLocalEntity->GetAbsOrigin().x;
		float flDeltaY = pBaseEntity->GetAbsOrigin().y - pLocalEntity->GetAbsOrigin().y;
		float flAngle = pLocalEntity->GetAbsAngles().y;
		float flYaw = ( flAngle )* ( PI / 180.0 );
		float flMainViewAngles_CosYaw = cos( flYaw );
		float flMainViewAngles_SinYaw = sin( flYaw );

		// rotate
		float x = flDeltaY * ( -flMainViewAngles_CosYaw ) + flDeltaX * flMainViewAngles_SinYaw;
		float y = flDeltaX * ( -flMainViewAngles_CosYaw ) - flDeltaY * flMainViewAngles_SinYaw;

		float flRange = pRange->getValue();

		if( fabs( x ) > flRange || fabs( y ) > flRange )
		{
			if( y > x )
			{
				if( y > -x )
				{
					x = flRange * x / y;
					y = flRange;
				}
				else
				{
					y = -flRange * y / x;
					x = -flRange;
				}
			}
			else
			{
				if( y > -x )
				{
					y = flRange * y / x;
					x = flRange;
				}
				else
				{
					x = -flRange * x / y;
					y = -flRange;
				}
			}
		}

		int	iScreenX = iCenterX + int( x / flRange * iSize );
		int iScreenY = iCenterY + int( y / flRange * iSize );

		Vector vLocalPos, vTargetPos;
		pLocalEntity->GetWorldSpaceCenter( vLocalPos );
		pBaseEntity->GetWorldSpaceCenter( vTargetPos );

		if( vLocalPos.z > vTargetPos.z + 80 )
		{
			gDrawManager.DrawString( "hud", iScreenX, iScreenY - 2, dwColor, "L" );
		}
		else if( vLocalPos.z < vTargetPos.z - 80 )
		{
			gDrawManager.DrawString( "hud", iScreenX, iScreenY - 2, dwColor, "H" );
		}
		else
		{
			gDrawManager.DrawRect( iScreenX - 3, iScreenY - 3, 7, 7, COLORCODE( 0, 0, 0, 255 ) );
			gDrawManager.DrawRect( iScreenX - 2, iScreenY - 2, 5, 5, dwColor );
		}
	}

	return;
}

F1_Point CRadar::getWidthHeight()
{
	return{ 2 * pSize->getValue(), 2 * pSize->getValue() };
}

F1_Rect CRadar::getBounds()
{
	return{ getPos().x, getPos().y, pSize->getValue() * 2, pSize->getValue() * 2 };
}

void CRadar::menuUpdate( F1_IConVar **pMenuArray, int &currIndex )
{
	pMenuArray[ currIndex++ ] = pRadarSwitch;

	if( pRadarSwitch->getValue() == true )
	{
		pMenuArray[ currIndex++ ] = pEnabledRadar;
		pMenuArray[ currIndex++ ] = pSize;
		pMenuArray[ currIndex++ ] = pRange;
	}
}