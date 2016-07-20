#include "CNoise.h"
#include "SDK.h"

const char *CNoise::name() const
{
	return "NOISEMAKER SPAM";
}

void CNoise::processCommand(CUserCmd *pUserCmd)
{
	if( enabled->getValue() == false )
		return;

	PVOID kv = keyVal::InitKeyValue();
	if(kv)
	{
		// the thing here doesnt matter.
		keyVal::SetName(kv, "use_action_slot_item_server", 0xDEADBEEF);
		gInts.Engine->ServerCmdKeyValues(kv);
		return;
	}
}

void CNoise::menuUpdate( F1_IConVar ** menuArray, int & currIndex )
{
	menuArray[ currIndex++ ];
}
