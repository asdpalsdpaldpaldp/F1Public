#include "CEsp.h"

#include "SDK.h"
#include "CDrawManager.h"
#include "Util.h"

#include "CTargetHelper.h"
#include "CPlayerManager.h"

CESP gEsp;

const char *CESP::name() const
{
	return "EXTRA-SENSORY PERCEPTION";
}

void CESP::processEntity( CBaseEntity *pBaseEntity )
{

	// no dormants
	if(pBaseEntity->IsDormant())
		return;

	Vector vecWorld, vecScreen;
	Vector min, max, origin, localOrigin;

	pBaseEntity->GetRenderBounds(min, max);

	origin = pBaseEntity->GetAbsOrigin();

	pBaseEntity->GetWorldSpaceCenter(vecWorld);

	if(!gDrawManager.WorldToScreen(vecWorld, vecScreen))
		return;

	DWORD teamColor = COLORCODE(0,0,0,255);

	DWORD playerColor = gPlayerManager.getColorForPlayer( pBaseEntity->GetIndex() );
	if( ( playerColor != 0xFFFFFFFF ) )
		teamColor = playerColor;
	else
		teamColor = teamColor = gDrawManager.dwGetTeamColor( pBaseEntity->GetTeam() );

	float distance = (localOrigin - origin).Length();

	//Draw on the player.

	classId id = pBaseEntity->GetClientClass()->iClassID;

	if(id == classId::CTFPlayer)
	{

		// no deads
		if(pBaseEntity->IsAlive() == false)
			return;

		player_info_t info;
		if(!gInts->Engine->GetPlayerInfo(pBaseEntity->GetIndex(), &info))
			return;

		if( hitboxes->getValue() == true )
		{
			for(int i = 0; i < 17; i++ )
				FrameHitbox( pBaseEntity, i );
		}

		if( renderBox->getValue() == true )
		{

			DynamicBox( pBaseEntity, teamColor );

			//int iRadius = 300 / distance;

			//Vector bot, top;
			//if( gDrawManager.WorldToScreen( Vector( origin.x, origin.y, origin.z + min.z ), bot ) && gDrawManager.WorldToScreen( Vector( origin.x, origin.y, origin.z + max.z ), top ) )
			//{
			//	float flHeight = bot.y - top.y;
			//	float flWidth = flHeight / 4.0f;

			//	//gDrawManager.DrawBox( vecWorld, teamColor, flWidth * 2, flHeight, flWidth );	

			//	//gDrawManager.DrawRadiusBox( top.x - flWidth, top.y, flWidth * 2, flHeight, 1, teamColor ); // player box.

			//}
		}

		if(renderGUID->getValue() == true)
		{
			gDrawManager.DrawString("esp", vecScreen.x, vecScreen.y, COLOR_OBJ, XorString("%s"), info.guid);
			vecScreen.y += gDrawManager.GetESPHeight();
		}

		if(renderName->getValue() == true)
		{
			gDrawManager.DrawString("esp", vecScreen.x, vecScreen.y, teamColor, XorString("%s"), info.name);
			vecScreen.y += gDrawManager.GetESPHeight();
		}

		if(renderHealth->getValue() == true)
		{
			gDrawManager.DrawString( "esp", vecScreen.x, vecScreen.y, redGreenGradiant(pBaseEntity->GetHealth(), getMaxHealth(pBaseEntity->GetClass())), XorString( "%i" ), pBaseEntity->GetHealth() /*gInts->GameResource->getHealth(index)*/ ); //Draw on the player.
			vecScreen.y += gDrawManager.GetESPHeight();
		}

		if(renderIndex->getValue() == true)
		{
			gDrawManager.DrawString( "esp", vecScreen.x, vecScreen.y, teamColor, XorString( "%i" ), pBaseEntity->GetIndex() ); //Draw on the player.
			vecScreen.y += gDrawManager.GetESPHeight();
		}

		if(renderViewESP->getValue() == true)
		{

			Vector angles = pBaseEntity->GetPrevLocalAngles();
			//gDrawManager.DrawString("esp", vecScreen.x, vecScreen.y, COLOR_OBJ, "%f, %f, %f", angles.x, angles.y, angles.z);
			//vecScreen.y += gDrawManager.GetESPHeight();
			Vector forward;
			AngleVectors(angles, &forward);
			Vector eyepos = pBaseEntity->GetAbsOrigin() + pBaseEntity->GetAbsOrigin();
			forward = forward * viewESPLength->getValue() + eyepos;

			// we cant use the '1 tick' trick here as in paintTraverse, we are run multiple times per tick!
			// we should use debugoverlay here for speed reasons, but also so that it doesnt show through walls
			//gInts->DebugOverlay->AddLineOverlay(eyepos, forward, RED(teamColor), GREEN(teamColor), BLUE(teamColor), false, 1);

			Vector screenForward, screenEyepos;
			if(gDrawManager.WorldToScreen(eyepos, screenEyepos) && gDrawManager.WorldToScreen(forward, screenForward))
			{
				gDrawManager.drawLine(screenForward.x, screenForward.y, screenEyepos.x, screenEyepos.y, teamColor);
			}
		}

		drawBoneEsp( pBaseEntity, teamColor );

	}
	else if(/*id == classId::CObjectDispenser || id == classId::CObjectSapper || id == classId::CObjectSentrygun || id == classId::CObjectTeleporter*/true)
	{
		// just draw the name now

		if( renderObjectID->getValue() == true )
		{
			gDrawManager.DrawString("esp", vecScreen.x, vecScreen.y, teamColor, XorString("%s"), pBaseEntity->GetClientClass()->chName);
			vecScreen.y += gDrawManager.GetESPHeight();
		}
	}
	return;
}

void CESP::menuUpdate( F1_IConVar ** menuArray, int & currIndex )
{
	menuArray[ currIndex++ ] = espSwitch;

	if(espSwitch->getValue() == true )
	{
		menuArray[ currIndex++ ] = renderBox;
		menuArray[ currIndex++ ] = renderBones;
		menuArray[ currIndex++ ] = hitboxes;
		menuArray[ currIndex++ ] = renderGUID;
		menuArray[ currIndex++ ] = renderName;
		menuArray[ currIndex++ ] = renderHealth;
		menuArray[ currIndex++ ] = renderIndex;
		menuArray[ currIndex++ ] = renderObjectID;
		menuArray[ currIndex++ ] = renderViewESP;
		menuArray[ currIndex++ ] = viewESPLength;
	}
}

void CESP::FrameHitbox( CBaseEntity *pBaseEntity, int iHitbox )
{
	const PDWORD pModel = pBaseEntity->GetModel();
	if(!pModel)
		return;

	DWORD *pStudioHdr = gInts->ModelInfo->GetStudiomodel(pModel);
	if(!pStudioHdr)
		return;

	/*mstudiohitboxset_t* pSet = pStudioHdr->pHitboxSet(pPlayer->GetHitboxSet());
	if(!pSet)
		return;

	mstudiobbox_t* pBox = pSet->pHitbox(iHitbox);
	if(!pBox)
		return;*/

	int HitboxSetIndex = *(int *)((DWORD)pStudioHdr + 0xB0);
	mstudiohitboxset_t *pSet = (mstudiohitboxset_t *)(((PBYTE)pStudioHdr) + HitboxSetIndex);

	auto pBox =  (mstudiobbox_t *)pSet->pHitbox(iHitbox);

	matrix3x4 vMatrix[128];

	if(!pBaseEntity->SetupBones(vMatrix, 128, 0x100, gInts->Globals->curtime))
		return;

	Vector vMin = pBox->bbmin;
	Vector vMax = pBox->bbmax;

	Vector vPointList[] =
	{
		Vector(vMin.x, vMin.y, vMin.z),
		Vector(vMin.x, vMax.y, vMin.z),
		Vector(vMax.x, vMax.y, vMin.z),
		Vector(vMax.x, vMin.y, vMin.z),
		Vector(vMax.x, vMax.y, vMax.z),
		Vector(vMin.x, vMax.y, vMax.z),
		Vector(vMin.x, vMin.y, vMax.z),
		Vector(vMax.x, vMin.y, vMax.z)
	};

	Vector vTransformed[8];

	for(int i = 0; i < 8; i++)
		VectorTransform(vPointList[i], vMatrix[pBox->bone], vTransformed[i]);

	gDrawManager.DrawBox(vTransformed, COLOR_OBJ);
}

void CESP::DynamicBox( CBaseEntity *pBaseEntity, DWORD dwColor )
{
	const matrix3x4& trans = pBaseEntity->GetRgflCoordinateFrame();

	Vector min = pBaseEntity->GetCollideableMins();
	Vector max = pBaseEntity->GetCollideableMaxs();

	Vector pointList[] = {
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z)
	};

	Vector transformed[8];

	for(int i = 0; i < 8; i++)
		VectorTransform(pointList[i], trans, transformed[i]);

	Vector flb, brt, blb, frt, frb, brb, blt, flt;

	if(!gDrawManager.WorldToScreen(transformed[3], flb) ||
		!gDrawManager.WorldToScreen(transformed[0], blb) ||
		!gDrawManager.WorldToScreen(transformed[2], frb) ||
		!gDrawManager.WorldToScreen(transformed[6], blt) ||
		!gDrawManager.WorldToScreen(transformed[5], brt) ||
		!gDrawManager.WorldToScreen(transformed[4], frt) ||
		!gDrawManager.WorldToScreen(transformed[1], brb) ||
		!gDrawManager.WorldToScreen(transformed[7], flt))
		return;

	Vector arr[] = {flb, brt, blb, frt, frb, brb, blt, flt};

	float left = flb.x;
	float top = flb.y;
	float right = flb.x;
	float bottom = flb.y;

	for( int i = 0; i < 8; i++ )
	{
		if( left > arr[ i ].x )
			left = arr[ i ].x;
		if( top < arr[ i ].y )
			top = arr[ i ].y;
		if( right < arr[ i ].x )
			right = arr[ i ].x;
		if( bottom > arr[ i ].y )
			bottom = arr[ i ].y;
	}

	float x = left;
	float y = bottom;
	float w = right - left;
	float h = top - bottom;

	// TODO change this to a convar
	if( false )
	{
		gDrawManager.DrawCornerBox( x - 1, y - 1, w + 2, h + 1, 3, 5, COLOR_BLACK );
		gDrawManager.DrawCornerBox( x, y, w, h - 1, 3, 5, dwColor );
	}
	else if(true )
	{
		gDrawManager.DrawBox( transformed, dwColor );
	}



	int health = pBaseEntity->GetHealth();

	tf_classes Class = pBaseEntity->GetClass();
	int maxHealth = getMaxHealth(Class);

	if(health > maxHealth)
		health = maxHealth;

	//int r = 255 - health * 2.55;
	//int g = health * 2.55;

	int healthBar = h / maxHealth * health;
	int healthBarDelta = h - healthBar;

	gDrawManager.OutlineRect(x - 5, y - 1, 5, h + 2, COLOR_BLACK);
	gDrawManager.DrawRect(x - 4, y + healthBarDelta, 3, healthBar, redGreenGradiant(health, maxHealth));
	//-------------------------------------------------------------------------------------------------------------
}

void CESP::drawBoneEsp( CBaseEntity *pBaseEntity, DWORD color )
{
	if( renderBones->getValue() )
	{

		static const tf_hitbox leftArm[ ] = { tf_hitbox::hand_L, tf_hitbox::lowerArm_L, tf_hitbox::upperArm_L, tf_hitbox::spine_2 };
		static const tf_hitbox rightArm[ ] = { tf_hitbox::hand_R, tf_hitbox::lowerArm_R, tf_hitbox::upperArm_R, tf_hitbox::spine_2 };
		static const tf_hitbox head[ ] = { tf_hitbox::head, tf_hitbox::spine_2, tf_hitbox::pelvis };
		static const tf_hitbox leftLeg[ ] = { tf_hitbox::foot_L, tf_hitbox::knee_L, tf_hitbox::pelvis };
		static const tf_hitbox rightLeg[ ] = { tf_hitbox::foot_R, tf_hitbox::knee_R, tf_hitbox::pelvis };

		const PDWORD pModel = pBaseEntity->GetModel();
		if( !pModel )
			return;

		DWORD *pStudioHdr = gInts->ModelInfo->GetStudiomodel( pModel );
		if( !pStudioHdr )
			return;

		/*mstudiohitboxset_t* pSet = pStudioHdr->pHitboxSet(pPlayer->GetHitboxSet());
		if(!pSet)
		return;

		mstudiobbox_t* pBox = pSet->pHitbox(iHitbox);
		if(!pBox)
		return;*/

		static matrix3x4 BoneToWorld[ 128 ];

		if( !pBaseEntity->SetupBones( BoneToWorld, 128, 0x100, 0 ) )
			return;

		int HitboxSetIndex = *( int * ) ( ( DWORD ) pStudioHdr + 0xB0 );
		mstudiohitboxset_t *pSet = ( mstudiohitboxset_t * ) ( ( ( PBYTE ) pStudioHdr ) + HitboxSetIndex );

		auto drawBoneChain = []( const tf_hitbox *bones, int count, mstudiohitboxset_t *pSet )
		{
			Vector startPos, endPos;
			Vector startWorld, endWorld;
			Vector Min, Max;

			for( int i = 0; i < count; i++ )
			{
				if( i != ( count - 1 ) )
				{
					if( i == 0 )
					{
						tf_hitbox h = bones[ 0 ];

						auto *pBox = pSet->pHitbox( ( int ) h );

						VectorTransform( pBox->bbmin, BoneToWorld[ pBox->bone ], Min );
						VectorTransform( pBox->bbmax, BoneToWorld[ pBox->bone ], Max );

						startPos = ( Min + Max ) / 2;
					}
					else
					{
						startPos = endPos;
					}


					tf_hitbox h = bones[ i + 1 ];

					auto *pBox = pSet->pHitbox( ( int ) h );

					VectorTransform( pBox->bbmin, BoneToWorld[ pBox->bone ], Min );
					VectorTransform( pBox->bbmax, BoneToWorld[ pBox->bone ], Max );

					endPos = ( Min + Max ) / 2;

					if( !gDrawManager.WorldToScreen( startPos, startWorld ) || !gDrawManager.WorldToScreen( endPos, endWorld ) )
						continue;

					gDrawManager.drawLine( startWorld.x, startWorld.y, endWorld.x, endWorld.y, COLORCODE( 255, 255, 255, 255 ) );
				}
			}

		};

		drawBoneChain( leftArm, 4, pSet );
		drawBoneChain( rightArm, 4, pSet );

		drawBoneChain( head, 3, pSet );

		drawBoneChain( leftLeg, 3, pSet );
		drawBoneChain( rightLeg, 3, pSet );

	}
}
