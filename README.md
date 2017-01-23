# F1Public
### Public tf2 hack

![alt text](http://i.imgur.com/t6F7Ceu.png "F1Public Logo")

This is the public version of my hack that i will be updating when i see fit. All the code here is the same as my own hack at one point.


Join the discord: https://discord.io/f12017


## Features
### Aimbot
* silent / non silent
* distance / aimbot targeting
* engine prediction
* projectile aimbot

### ESP
* box / 3d box
* health (and health bar)
* name
* guuid
* index
* view

### Triggerbot
* hitbox or hitgroup
* zoomed only

### Player list
* Friend 
* Enemy
* Aimbot target

# Adding more to the hack
Unless it is already part of the hack, modules can be added in the following way

Module header file:
```cpp
class Module : IHack<Module> // Module needs to be the same in both
{
  // potential member variables
  F1_ConVar<Switch> moduleSwitch("module", false);
  F1_ConVar<int> moduleInt(" - int", default, min, max, step);
  F1_ConVar<bool> moduleBool("- bool", startingValue);
  F1_ConVar<float> moduleFloat(" - float", default, min, max, step);
  
  // called in createmove after local player prediction
  void processCommand(CUserCmd *pUserCmd);
  
  // called in createmove after local player prediction
  void processCommandBeforePred(CUserCmd *pUserCmd);
  
  // called in painttraverse
  bool paint();
  
  // called in the entity loop in painttraverse
  void processEntity(CBaseEntity *pBaseEntity)
  
  // called when the menu updates
  void menuUpdate(F1_IConVar **menuArray, int &currIndex);
}
extern Module gModule;
```
Module cpp file:
```cpp

#include <Module.h>

Module gModule

void Module::processCommand(CUserCmd *pUserCmd)
{
  // do stuff
}

void Module::processCommandBeforePred(CUserCmd *pUserCmd)
{
  // do stuff
}

bool Module::paint()
{
  // do stuff
  return false;
}

void Module::processEntity(CBaseEntity *pBaseEntity)
{
  // do stuff
}

void Module::menuUpdate(F1_IConVar **menuArray, int &currIndex)
{
  menuArray[currIndex++] = moduleSwitch;

  if(moduleSwitch.getValue() == true )
  {
    menuArray[currIndex++] = moduleInt;
    menuArray[currIndex++] = moduleFloat;
    menuArray[currIndex++] = moduleBool;
  }
}
```

In order to make the hack active, add it to the active hacks definition in CHack.h:
```cpp
#define ACTIVE_HACKS \
	gAimbot, gAnnouncer, gAntiaim, *CAntiSmac::getInst(), \
	gAutoAirblast, /*gBackstab,*/ gEsp, /*gGlow, gNokick, */gPlayerManager, \
	gPureBypass, gRadar, gTrigger, gMisc, gHack, gModule
```

## Credits wow

#### gir489
creater of the darkstorm base and the noisemaker spam code
##### learn_more for the signature scanning function's from ds2015

#### Volvo
source sdk and
leaked sdk

#### Altimor
netvar class that i have used in all my projects

#### opensteamworks team
all of the steam/ directory

#### If I have used some of your code and haven't credited you, please tell me (or make a pull request)
