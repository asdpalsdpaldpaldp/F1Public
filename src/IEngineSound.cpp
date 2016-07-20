#include "IEngineSound.h"

#include "SDK.h"

void CRecipientFilter::AddRecipient(CBaseEntity *player)
{
	//Assert(player);

	if(!player)
		return;

	int index = player->GetIndex();

	// If we're predicting and this is not the first time we've predicted this sound
	//  then don't send it to the local player again.
	//if(m_bUsingPredictionRules)
	//{
	//	// Only add local player if this is the first time doing prediction
	//	if(g_RecipientFilterPredictionSystem.GetSuppressHost() == player)
	//	{
	//		return;
	//	}
	//}

	// Already in list
	//if(m_Recipients.Find(index) != m_Recipients.InvalidIndex())
	//	return;

	m_Recipients.AddToTail(index);
}