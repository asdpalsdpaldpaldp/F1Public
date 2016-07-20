#pragma once

//#include "Trace.h"

#include "IHack.h"
#include "Vector.h"
#include "SDK.h"

class CTrigger : public IHack
{

	//bool enabled, hitAll, useHitbox, zoomedOnly;
	//int hitbox, hitgroup;

	F1_ConVar<Switch> *triggerSwitch;
	F1_ConVar<bool> *enabled;
	F1_ConVar<Enum<tf_hitbox>> *hitbox;
	F1_ConVar<bool> *zoomedOnly;

	//var enabled_bool{"Enabled", &enabled};
	//var hitAll_bool{"Hit all", &hitAll};
	//var hitbox_bool{"Hitbox?", &useHitbox};
	//var hitbox_int{"Hitbox Number", &hitgroup, 0, 17, 1, 0};
	//var hitgroup_int{"Hitgroup Number", &hitgroup, 0, 7, 1, 0};
	//var zoomed_bool{"Zoomed Only", &zoomedOnly};

public:
	CTrigger()
	{
		//variables.push_back(enabled_bool);
		//variables.push_back(hitAll_bool);
		//variables.push_back(hitbox_bool);
		//variables.push_back(hitbox_int);
		//variables.push_back(hitgroup_int);
		//variables.push_back(zoomed_bool);

		triggerSwitch = new F1_ConVar<Switch>( "Triggerbot", false );
		enabled = new F1_ConVar<bool>( " - Enabled", false );
		hitbox = new F1_ConVar<Enum<tf_hitbox>>( " - Hitbox", { tf_hitbox::head,
		{
			{ tf_hitbox::foot_L, "Left foot" },
			{ tf_hitbox::foot_R, "Right foot" },
			{ tf_hitbox::hand_L, "Left Hand" },
			{ tf_hitbox::hand_R, "Right hand" },
			{ tf_hitbox::head, "Head" },
			{ tf_hitbox::hip_L, "Left hip" },
			{ tf_hitbox::hip_R, "Right hip" },
			{ tf_hitbox::knee_L, "Left knee" },
			{ tf_hitbox::knee_R, "Right knee" },
			{ tf_hitbox::lowerArm_L, "Lower left arm" },
			{ tf_hitbox::lowerArm_R, "Lower right arm" },
			{ tf_hitbox::pelvis, "Pelvis" },
			{ tf_hitbox::spine_0, "Spine 0" },
			{ tf_hitbox::spine_1, "Spine 1" },
			{ tf_hitbox::spine_2, "Spine 2" },
			{ tf_hitbox::spine_3, "Spine 3" },
			{ tf_hitbox::upperArm_L, "Upper left arm" },
			{ tf_hitbox::upperArm_R, "Upper right arm" },
		}
		}, tf_hitbox::head, tf_hitbox::foot_R );
		zoomedOnly = new F1_ConVar<bool>( " - Zoomed only", false );
	}

	const char *name() const override;
	bool paint() override;
	void processCommand(CUserCmd *pUserCmd) override;

	void menuUpdate( F1_IConVar **menuArray, int &currIndex ) override;

private:
	// Vector curr_angles, curr_direction, curr_ray, curr_endpos, curr_pos;

	// int curr_hitbox, curr_hitgroup, curr_physicsbone, curr_class_id;

	// bool curr_ent_null;
};
