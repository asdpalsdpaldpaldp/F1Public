#include "CGlow.h"

#include "SDK.h"
#include "CDrawManager.h"
#include "CEntity.h"
#include "Util.h"
#include "Color.h"

CGlow gGlow;

// team 3: 0.490000 0.660000 0.770000 blue
// team 2: 0.740000 0.230000 0.230000 red

// these are some HORRIBLE MACROS
// one of the worst things i have ever done
#define BLUCOLORFLOAT 0.49f, 0.66f, 0.77f
#define REDCOLORFLOAT 0.74f, 0.23f, 0.23f

int CGlowManager::registerGlowObject(CBaseEntity *ent, float r, float g, float b, float a, bool bRenderWhenOccluded, bool bRenderWhenUnoccluded, int nSplitScreenSlot)
{
	// assumes the glow object is real

	//typedef int(__thiscall * registerFn)(CGlowManager *, CBaseEntity *, Vector &, float, bool, bool, int);
	//static DWORD dwFn			= gSignatures.GetClientSignature("55 8B EC 51 53 56 8B F1 57 8B 5E 14");
	//static registerFn Register = (registerFn)dwFn;

	//return Register(pGlowObjectManger, ent, Vector{r, g, b}, a, bRenderWhenOccluded, bRenderWhenUnoccluded, nSplitScreenSlot);

	int nIndex;
	if(m_nFirstFreeSlot == GlowObjectDefinition_t::END_OF_FREE_LIST)
	{
		nIndex = glowObjects.AddToTail();
	}
	else
	{
		nIndex = m_nFirstFreeSlot;
		m_nFirstFreeSlot = glowObjects[nIndex].m_nNextFreeSlot;
	}

	//glowObjects[nIndex].hEntity = ent;
	glowObjects[nIndex].r = r;
	glowObjects[nIndex].g = g;
	glowObjects[nIndex].b = b;
	glowObjects[nIndex].a = a;
	glowObjects[nIndex].m_bRenderWhenOccluded = bRenderWhenOccluded;
	glowObjects[nIndex].m_bRenderWhenUnoccluded = bRenderWhenUnoccluded;
	glowObjects[nIndex].m_nSplitScreenSlot = nSplitScreenSlot;
	glowObjects[nIndex].m_nNextFreeSlot = GlowObjectDefinition_t::ENTRY_IN_USE;

	return nIndex;
}

void CGlowManager::unregisterGlowObject(int glowIndex)
{
	glowObjects[glowIndex].m_nNextFreeSlot = m_nFirstFreeSlot;
	glowObjects[glowIndex].hEntity = NULL;
	m_nFirstFreeSlot = glowIndex;
}

const char *CGlow::name() const { return "GLOW"; }

void CGlow::init()
{
	// get the glow manager from this sig ( which is 2 bytes into the sig )
	DWORD dwGlowLoc = gSignatures.GetClientSignature("8B 0D ? ? ? ? A1 ? ? ? ? 56 8B 37") + 0x2;
	XASSERT(dwGlowLoc);
	pGlowObjectManger = *(CGlowManager **)dwGlowLoc;
	XASSERT(pGlowObjectManger);

	return;
}

bool CGlow::paint()
{
	if(useTeamColors->getValue() || enabled->getValue() == false)
		return true;

	
	for(int i = 0; i < pGlowObjectManger->glowObjects.Count(); i++)
	{
		auto &glowObj = pGlowObjectManger->glowObjects[ i ];
		if(glowObj.hEntity.GetEntryIndex() != INVALID_EHANDLE_INDEX)
		{
			//CEntity<> ent{glowObj.hEntity.GetEntryIndex()};

			CBaseEntity *pBaseEntity = glowObj.hEntity.Get();

			if( pBaseEntity == nullptr)
				continue;

			if( pBaseEntity->IsDormant() || pBaseEntity->IsAlive() == false)
			{
				// unregister object
				pGlowObjectManger->unregisterGlowObject( i );
				continue;
			}

			int health = pBaseEntity->GetHealth();
			int maxHealth = getMaxHealth( pBaseEntity->GetClass() );

			DWORD dwColor = redGreenGradiant(health, maxHealth);

			// alpha is always 255
			glowObj.a = 1.0;
			glowObj.r = float(RED(dwColor)) / 255.0f;
			glowObj.g = float( GREEN(dwColor) ) / 255.0f;
			glowObj.b = float( BLUE(dwColor) ) / 255.0f;

			//Log::Console("team %d: %f %f %f", ent.get<int>(gEntVars.iTeam), glowObj.r, glowObj.g, glowObj.b);
		}
	}
	

	return true;
}

void CGlow::processEntity( CBaseEntity *pBaseEntity )
{
	// no nulls
	//if( pBaseEntity == NULL )
	//{
	//	if( glowObjects[ pBaseEntity->GetIndex() ] )
	//		glowObjects[ pBaseEntity->GetIndex() ] = false;
	//	return;
	//}

	if( !enabled->getValue() )
		return;

	classId id = pBaseEntity->GetClientClass()->iClassID;

	CBaseCombatCharacter *pBaseCharacter = ( CBaseCombatCharacter * ) pBaseEntity;

	if(id == classId::CTFPlayer)
	{
		// no dormants or deads - included check for if glow is disabled here since we have to tell the engine to stop the glow rather than just ending the loop
		if( pBaseEntity->IsDormant() || pBaseEntity->IsAlive() == false || !enabled ||
		   (useTeamColors->getValue() && gLocalPlayerVars.team == pBaseEntity->GetTeam()))
		{
			//pBaseCharacter->SetGlowEnabled( false );
			//( ( CBaseCombatCharacter * ) pBaseEntity )->DestroyGlowEffect();
			//pGlowObjectManger->unregisterGlowObject()
			return;
		}

		// if glow is not enabled
		if( pBaseCharacter->IsGlowEnabled() )
			return;

		// enable glow
		pBaseCharacter->SetGlowEnabled( true );
		// update the glow
		pBaseCharacter->UpdateGlowEffect();

		return;
	}
	// TODO replace with entity traits when that is done
	//else if(id == classId::CObjectDispenser || id == classId::CObjectSentrygun || id == classId::CObjectTeleporter || id == classId::CBaseObject /*||
	//		id == classId::CTFProjectile_Arrow || id == classId::CTFProjectile_Cleaver || id == classId::CTFProjectile_Flare ||
	//		id == classId::CTFProjectile_Rocket || id == classId::CTFProjectile_SentryRocket || id == classId::CTFGrenadePipebombProjectile*/)
	//{

	//	if(!enabled)
	//	{
	//		if(glowObjects[index] != -1 || player->IsDormant())
	//		{
	//			pGlowObjectManger->unregisterGlowObject(glowObjects[index]);
	//			glowObjects[index] = -1;
	//		}
	//	}
	//	else
	//	{
	//		int team = player.get<int>(gEntVars.iTeam);
	//		// create a glow object for this entity
	//		if(glowObjects[index] == -1 && (teamColor && gLocalPlayerVars.team == team))
	//		{
	//			if(team == 3)
	//			{
	//				glowObjects[index] = pGlowObjectManger->registerGlowObject(player.castToPointer<CBaseEntity>(), BLUCOLORFLOAT, 1.0f, true, true, -1);
	//			}
	//			else if(team == 2)
	//			{
	//				glowObjects[index] = pGlowObjectManger->registerGlowObject(player.castToPointer<CBaseEntity>(), REDCOLORFLOAT, 1.0f, true, true, -1);
	//			}
	//		}
	//		else if(player->IsDormant())
	//		{
	//			if(glowObjects[index] != -1)
	//			{
	//				pGlowObjectManger->unregisterGlowObject(glowObjects[index]);
	//				glowObjects[index] = -1;	
	//			}
	//		}
	//	}
	//}
	return;
}

void CGlow::menuUpdate( F1_IConVar **menuArray, int &currIndex )
{
	menuArray[ currIndex++ ] = glowSwitch;
	if(glowSwitch->getValue() )
	{
		menuArray[ currIndex++ ] = enabled;
		menuArray[ currIndex++ ] = enemyOnly;
		menuArray[ currIndex++ ] = useTeamColors;
	}
}
