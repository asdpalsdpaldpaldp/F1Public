#include "CAntiaim.h"
#include "CAimHelper.h"

#include "SDK.h"

CAntiaim gAntiaim;

const char *CAntiaim::name() const
{
	return "ANTI-AIM";
}

void CAntiaim::processCommand(CUserCmd *pUserCmd)
{
	if(!enabled->getValue())
		return;
	//if ( pUserCmd->buttons & IN_ATTACK )
	//	return true;

	//pUserCmd->viewangles.z -= 180;
	//newAngles.y -= 180;
	//pUserCmd->forwardmove = -pUserCmd->forwardmove;

	//if(!(pUserCmd->buttons & IN_ATTACK))
	//pUserCmd->sidemove = -pUserCmd->sidemove;
	//pUserCmd->forwardmove = -pUserCmd->forwardmove;

	if((pUserCmd->buttons & IN_ATTACK) == false)
	{

		Vector newAngles = pUserCmd->viewangles;

		if(fakeUp->getValue() == true)
		{
			newAngles.x = -271;
			//invert movement for movement fix while using this angle
			//pUserCmd->forwardmove = -pUserCmd->forwardmove;
			//pUserCmd->sidemove = -pUserCmd->sidemove;
			//return true;
		}

		if(fakeDown->getValue() == true)
		{
			newAngles.x = 271;
			//return true;
		}

		if(lisp->getValue() == true)
		{

			//newAngles.x = 36000180;
			//TODO, make movement fix for this
			//pUserCmd->forwardmove = -pUserCmd->forwardmove;
			//pUserCmd->sidemove = -pUserCmd->sidemove;

			static bool ySwitch = false;

			if( ySwitch )
				newAngles.y += 36000180;
			else
				newAngles.y -= 36000180;

			ySwitch = !ySwitch;
		}

		if(faceBackwards->getValue() == true)
		{
			newAngles.y -= 180;
			//pUserCmd->forwardmove = -pUserCmd->forwardmove;
			//pUserCmd->sidemove = -pUserCmd->sidemove;
			//return true;
		}

		if(spinBot->getValue() == true)
		{
			//fix movement
			//pUserCmd->forwardmove = -pUserCmd->forwardmove;
			//pUserCmd->sidemove = -pUserCmd->sidemove;

			int random = 160 + rand() % spinSpeed->getValue();
			static float current_y = newAngles.y;
			current_y += random;
			newAngles.y = current_y;
			//return true;
		}

		if(jitter->getValue() == true)
		{
			//fix movement
			//pUserCmd->forwardmove = -pUserCmd->forwardmove;
			//pUserCmd->sidemove = -pUserCmd->sidemove;

			int random = rand() % 100;

			// Small chance of starting fowards
			if(random < 98)
				// Look backwards
				newAngles.y -= 180;

			// Some gitter
			if(random < 15)
			{
				float change = -70 + (rand() % (int)(140 + 1));
				newAngles.y += change;
			}
			if(random == 69)
			{
				float change = -90 + (rand() % (int)(180 + 1));
				newAngles.y += change;
				//return true;
			}
		}

		silentMovementFix(pUserCmd, newAngles);

		pUserCmd->viewangles = newAngles;
	}
		
	//gInts->Engine->SetViewAngles(pUserCmd->viewangles);

	return;
}

void CAntiaim::menuUpdate( F1_IConVar **menuArray, int & currIndex )
{
	menuArray[ currIndex++ ] = AASwitch;
	
	if(AASwitch->getValue() == true )
	{
		menuArray[ currIndex++ ] = enabled;
		menuArray[ currIndex++ ] = fakeUp;
		menuArray[ currIndex++ ] = fakeDown;
		menuArray[ currIndex++ ] = faceBackwards;
		menuArray[ currIndex++ ] = spinBot;
		menuArray[ currIndex++ ] = spinSpeed;
		menuArray[ currIndex++ ] = jitter;
		menuArray[ currIndex++ ] = lisp;
	}
}
