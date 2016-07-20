#pragma once

//===================================================================================================
enum playercontrols
{
	IN_ATTACK = ( 1 << 0 ),
	IN_JUMP = ( 1 << 1 ),
	IN_DUCK = ( 1 << 2 ),
	IN_FORWARD = ( 1 << 3 ),
	IN_BACK = ( 1 << 4 ),
	IN_USE = ( 1 << 5 ),
	IN_CANCEL = ( 1 << 6 ),
	IN_LEFT = ( 1 << 7 ),
	IN_RIGHT = ( 1 << 8 ),
	IN_MOVELEFT = ( 1 << 9 ),
	IN_MOVERIGHT = ( 1 << 10 ),
	IN_ATTACK2 = ( 1 << 11 ),
	IN_RUN = ( 1 << 12 ),
	IN_RELOAD = ( 1 << 13 ),
	IN_ALT1 = ( 1 << 14 ),
	IN_ALT2 = ( 1 << 15 ),
	IN_SCORE = ( 1 << 16 ), // Used by client.dll for when scoreboard is held down
	IN_SPEED = ( 1 << 17 ), // Player is holding the speed key
	IN_WALK = ( 1 << 18 ), // Player holding walk key
	IN_ZOOM = ( 1 << 19 ), // Zoom key for HUD zoom
	IN_WEAPON1 = ( 1 << 20 ), // weapon defines these bits
	IN_WEAPON2 = ( 1 << 21 ), // weapon defines these bits
	IN_BULLRUSH = ( 1 << 22 ),
};
//===================================================================================================
enum tf_cond : int
{
	TFCond_Slowed = ( 1 << 0 ),  // Toggled when a player is slowed down.
	TFCond_Zoomed = ( 1 << 1 ),  // Toggled when a player is zoomed.
	TFCond_Disguising = ( 1 << 2 ),  // Toggled when a Spy is disguising.
	TFCond_Disguised = ( 1 << 3 ),  // Toggled when a Spy is disguised.
	TFCond_Cloaked = ( 1 << 4 ),  // Toggled when a Spy is invisible.
	TFCond_Ubercharged = ( 1 << 5 ),  // Toggled when a player is uberCharged.
	TFCond_TeleportedGlow = ( 1 << 6 ),  // Toggled when someone leaves a teleporter and has glow beneath their feet.
	TFCond_Taunting = ( 1 << 7 ),  // Toggled when a player is taunting.
	TFCond_UberchargeFading = ( 1 << 8 ),  // Toggled when the uberCharge is fading.
	TFCond_CloakFlicker = ( 1 << 9 ),  // Toggled when a Spy is visible during cloak.
	TFCond_Teleporting = ( 1 << 10 ), // Only activates for a brief second when the player is being teleported; not very useful.
	TFCond_Kritzkrieged = ( 1 << 11 ), // Toggled when a player is being crit buffed by the KritzKrieg.
	TFCond_TmpDamageBonus = ( 1 << 12 ), // Unknown what this is for. Name taken from the AlliedModders SDK.
	TFCond_DeadRingered = ( 1 << 13 ), // Toggled when a player is taking reduced damage from the Deadringer.
	TFCond_Bonked = ( 1 << 14 ), // Toggled when a player is under the effects of The Bonk! Atomic Punch.
	TFCond_Stunned = ( 1 << 15 ), // Toggled when a player's speed is reduced from airblast or a Sandman ball.
	TFCond_Buffed = ( 1 << 16 ), // Toggled when a player is within range of an activated Buff Banner.
	TFCond_Charging = ( 1 << 17 ), // Toggled when a Demoman charges with the shield.
	TFCond_DemoBuff = ( 1 << 18 ), // Toggled when a Demoman has heads from the Eyelander.
	TFCond_CritCola = ( 1 << 19 ), // Toggled when the player is under the effect of The Crit-a-Cola.
	TFCond_InHealRadius = ( 1 << 20 ), // Unused condition, name taken from AlliedModders SDK.
	TFCond_Healing = ( 1 << 21 ), // Toggled when someone is being healed by a medic or a dispenser.
	TFCond_OnFire = ( 1 << 22 ), // Toggled when a player is on fire.
	TFCond_Overhealed = ( 1 << 23 ), // Toggled when a player has >100% health.
	TFCond_Jarated = ( 1 << 24 ), // Toggled when a player is hit with a Sniper's Jarate.
	TFCond_Bleeding = ( 1 << 25 ), // Toggled when a player is taking bleeding damage.
	TFCond_DefenseBuffed = ( 1 << 26 ), // Toggled when a player is within range of an activated Battalion's Backup.
	TFCond_Milked = ( 1 << 27 ), // Player was hit with a jar of Mad Milk.
	TFCond_MegaHeal = ( 1 << 28 ), // Player is under the effect of Quick-Fix charge.
	TFCond_RegenBuffed = ( 1 << 29 ), // Toggled when a player is within a Concheror's range.
	TFCond_MarkedForDeath = ( 1 << 30 ), // Player is marked for death by a Fan O'War hit. Effects are similar to TFCond_Jarated.
	TFCond_NoHealingDamageBuff = ( 1 << 31 ), // Unknown what this is used for.

	TFCond_MiniCrits = ( TFCond_Buffed | TFCond_CritCola ),
	TFCond_IgnoreStates = ( TFCond_Ubercharged | TFCond_Bonked ),
};
enum tf_cond_ex : int
{
	TFCondEx_SpeedBuffAlly = ( 1 << 0 ),  // Toggled when a player gets hit with the disciplinary action.
	TFCondEx_HalloweenCritCandy = ( 1 << 1 ),  // Only for Scream Fortress event maps that drop crit candy.
	TFCondEx_CritCanteen = ( 1 << 2 ),  // Player is getting a crit boost from a MVM canteen.
	TFCondEx_CritDemoCharge = ( 1 << 3 ),  // From demo's shield
	TFCondEx_CritHype = ( 1 << 4 ),  // Soda Popper crits.
	TFCondEx_CritOnFirstBlood = ( 1 << 5 ),  // Arena first blood crit buff.
	TFCondEx_CritOnWin = ( 1 << 6 ),  // End of round crits.
	TFCondEx_CritOnFlagCapture = ( 1 << 7 ),  // CTF intelligence capture crits.
	TFCondEx_CritOnKill = ( 1 << 8 ),  // Unknown what this is for.
	TFCondEx_RestrictToMelee = ( 1 << 9 ),  // Unknown what this is for.
	TFCondEx_DefenseBuffNoCritBlock = ( 1 << 10 ), // MvM Buff.
	TFCondEx_Reprogrammed = ( 1 << 11 ), // MvM Bot has been reprogrammed.
	TFCondEx_PyroCrits = ( 1 << 12 ), // Player is getting crits from the Mmmph charge.
	TFCondEx_PyroHeal = ( 1 << 13 ), // Player is being healed from the Mmmph charge.
	TFCondEx_FocusBuff = ( 1 << 14 ), // Player is getting a focus buff.
	TFCondEx_DisguisedRemoved = ( 1 << 15 ), // Disguised remove from a bot.
	TFCondEx_MarkedForDeathSilent = ( 1 << 16 ), // Player is under the effects of the Escape Plan/Equalizer or GRU.
	TFCondEx_DisguisedAsDispenser = ( 1 << 17 ), // Bot is disguised as dispenser.
	TFCondEx_Sapped = ( 1 << 18 ), // MvM bot is being sapped.
	TFCondEx_UberchargedHidden = ( 1 << 19 ), // MvM Related
	TFCondEx_UberchargedCanteen = ( 1 << 20 ), // Player is receiving uberCharge from a canteen.
	TFCondEx_HalloweenBombHead = ( 1 << 21 ), // Player has a bomb on their head from Merasmus.
	TFCondEx_HalloweenThriller = ( 1 << 22 ), // Players are forced to dance from Merasmus.
	TFCondEx_BulletCharge = ( 1 << 26 ), // Player is receiving 75% reduced damage from bullets.
	TFCondEx_ExplosiveCharge = ( 1 << 27 ), // Player is receiving 75% reduced damage from explosives.
	TFCondEx_FireCharge = ( 1 << 28 ), // Player is receiving 75% reduced damage from fire.
	TFCondEx_BulletResistance = ( 1 << 29 ), // Player is receiving 10% reduced damage from bullets.
	TFCondEx_ExplosiveResistance = ( 1 << 30 ), // Player is receiving 10% reduced damage from explosives.
	TFCondEx_FireResistance = ( 1 << 31 ), // Player is receiving 10% reduced damage from fire.

	TFCondEx_IgnoreStates = ( TFCondEx_PyroHeal )
};
enum tf_cond_ex2 : int
{
	TFCondEx2_Stealthed = ( 1 << 0 ),
	TFCondEx2_MedigunDebuff = ( 1 << 1 ),
	TFCondEx2_StealthedUserBuffFade = ( 1 << 2 ),
	TFCondEx2_BulletImmune = ( 1 << 3 ),
	TFCondEx2_BlastImmune = ( 1 << 4 ),
	TFCondEx2_FireImmune = ( 1 << 5 ),
	TFCondEx2_PreventDeath = ( 1 << 6 ),
	TFCondEx2_MVMBotRadiowave = ( 1 << 7 ),
	TFCondEx2_HalloweenSpeedBoost = ( 1 << 8 ),  // Wheel has granted player speed boost.
	TFCondEx2_HalloweenQuickHeal = ( 1 << 9 ),  // Wheel has granted player quick heal.
	TFCondEx2_HalloweenGiant = ( 1 << 10 ), // Wheel has granted player giant mode.
	TFCondEx2_HalloweenTiny = ( 1 << 11 ), // Wheel has granted player tiny mode.
	TFCondEx2_HalloweenInHell = ( 1 << 12 ), // Wheel has granted player in hell mode.
	TFCondEx2_HalloweenGhostMode = ( 1 << 13 ), // Wheel has granted player ghost mode.
	TFCondEx2_Parachute = ( 1 << 16 ), // Player has deployed the BASE Jumper.
	TFCondEx2_BlastJumping = ( 1 << 17 ), // Player has sticky or rocket jumped.
	TFCondEx2_BumperCar = ( 1 << 18 ),
	TFCondEx2_83 = ( 1 << 19 ),
	TFCondEx2_84 = ( 1 << 20 ),
	TFCondEx2_85 = ( 1 << 21 ),
	TFCondEx2_86 = ( 1 << 22 ),
	TFCondEx2_87 = ( 1 << 23 ),
	TFCondEx2_88 = ( 1 << 24 ),
	TFCondEx2_89 = ( 1 << 25 ),
	TFCondEx2_90 = ( 1 << 26 ),
	TFCondEx2_91 = ( 1 << 27 ),
	TFCondEx2_92 = ( 1 << 28 ),
	TFCondEx2_93 = ( 1 << 29 ),
	TFCondEx2_94 = ( 1 << 30 ),
	TFCondEx2_95 = ( 1 << 31 ),
};
enum tf_cond_ex3 : int
{
	TFCondEx2_96 = ( 1 << 0 ),
	TFCondEx2_97 = ( 1 << 1 ),
	TFCondEx2_98 = ( 1 << 2 ),
	TFCondEx2_99 = ( 1 << 3 ),
	TFCondEx2_100 = ( 1 << 4 ),
	TFCondEx2_101 = ( 1 << 5 ),
	TFCondEx2_102 = ( 1 << 6 ),
	TFCondEx2_103 = ( 1 << 7 ),
	TFCondEx2_104 = ( 1 << 8 ),
	TFCondEx2_105 = ( 1 << 9 ),
	TFCondEx2_106 = ( 1 << 10 ),
	TFCondEx2_107 = ( 1 << 11 ),
	TFCondEx2_108 = ( 1 << 12 ),
	TFCondEx2_109 = ( 1 << 13 ),
	TFCondEx2_110 = ( 1 << 14 ),
	TFCondEx2_111 = ( 1 << 15 ),
	TFCondEx2_112 = ( 1 << 16 ),
	TFCondEx2_113 = ( 1 << 17 ),
	TFCondEx2_114 = ( 1 << 18 ), // team glow
	TFCondEx2_115 = ( 1 << 19 ),
	/*
	TFCondEx2_116 = (1 << 20),
	TFCondEx2_117= (1 << 21),
	TFCondEx2_118 = (1 << 22),
	TFCondEx2_119 = (1 << 23),
	TFCondEx2_12 = (1 << 24),
	TFCondEx2_12 = (1 << 25),
	TFCondEx2_12 = (1 << 26),
	TFCondEx2_12 = (1 << 27),
	TFCondEx2_12 = (1 << 28),
	TFCondEx2_12 = (1 << 29),
	TFCondEx2_12 = (1 << 30),
	TFCondEx2_12 = (1 << 31),
	*/
};
//===================================================================================================
enum class tf_classes
{
	TF2_Invalid = -1,
	TF2_Scout = 1,
	TF2_Sniper = 2,
	TF2_Soldier = 3,
	TF2_Demoman = 4,
	TF2_Medic = 5,
	TF2_Heavy = 6,
	TF2_Pyro = 7,
	TF2_Spy = 8,
	TF2_Engineer = 9,
};
//===================================================================================================
enum source_lifestates
{
	LIFE_ALIVE,
	LIFE_DYING,
	LIFE_DEAD,
	LIFE_RESPAWNABLE,
	LIFE_DISCARDBODY,
};
//===================================================================================================
enum class ClientFrameStage_t
{
	FRAME_UNDEFINED = -1, // (haven't run any frames yet)
	FRAME_START,

	// A network packet is being recieved
	FRAME_NET_UPDATE_START,
	// Data has been received and we're going to start calling PostDataUpdate
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	// Data has been received and we've called PostDataUpdate on all data recipients
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	// We've received all packets, we can now do interpolation, prediction, etc..
	FRAME_NET_UPDATE_END,

	// We're about to start rendering the scene
	FRAME_RENDER_START,
	// We've finished rendering the scene.
	FRAME_RENDER_END
};
//===================================================================================================
enum class ButtonCode_t
{
	BUTTON_CODE_INVALID = -1,
	BUTTON_CODE_NONE = 0,

	KEY_FIRST = 0,

	KEY_NONE = KEY_FIRST,
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_PAD_0,
	KEY_PAD_1,
	KEY_PAD_2,
	KEY_PAD_3,
	KEY_PAD_4,
	KEY_PAD_5,
	KEY_PAD_6,
	KEY_PAD_7,
	KEY_PAD_8,
	KEY_PAD_9,
	KEY_PAD_DIVIDE,
	KEY_PAD_MULTIPLY,
	KEY_PAD_MINUS,
	KEY_PAD_PLUS,
	KEY_PAD_ENTER,
	KEY_PAD_DECIMAL,
	KEY_LBRACKET,
	KEY_RBRACKET,
	KEY_SEMICOLON,
	KEY_APOSTROPHE,
	KEY_BACKQUOTE,
	KEY_COMMA,
	KEY_PERIOD,
	KEY_SLASH,
	KEY_BACKSLASH,
	KEY_MINUS,
	KEY_EQUAL,
	KEY_ENTER,
	KEY_SPACE,
	KEY_BACKSPACE,
	KEY_TAB,
	KEY_CAPSLOCK,
	KEY_NUMLOCK,
	KEY_ESCAPE,
	KEY_SCROLLLOCK,
	KEY_INSERT,
	KEY_DELETE,
	KEY_HOME,
	KEY_END,
	KEY_PAGEUP,
	KEY_PAGEDOWN,
	KEY_BREAK,
	KEY_LSHIFT,
	KEY_RSHIFT,
	KEY_LALT,
	KEY_RALT,
	KEY_LCONTROL,
	KEY_RCONTROL,
	KEY_LWIN,
	KEY_RWIN,
	KEY_APP,
	KEY_UP,
	KEY_LEFT,
	KEY_DOWN,
	KEY_RIGHT,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_CAPSLOCKTOGGLE,
	KEY_NUMLOCKTOGGLE,
	KEY_SCROLLLOCKTOGGLE,

	KEY_LAST = KEY_SCROLLLOCKTOGGLE,
	KEY_COUNT = KEY_LAST - KEY_FIRST + 1,

	// Mouse
	MOUSE_FIRST = KEY_LAST + 1,

	MOUSE_LEFT = MOUSE_FIRST,
	MOUSE_RIGHT,
	MOUSE_MIDDLE,
	MOUSE_4,
	MOUSE_5,
	MOUSE_WHEEL_UP,   // A fake button which is 'pressed' and 'released' when the wheel is moved up
	MOUSE_WHEEL_DOWN, // A fake button which is 'pressed' and 'released' when the wheel is moved down

	MOUSE_LAST = MOUSE_WHEEL_DOWN,
	MOUSE_COUNT = MOUSE_LAST - MOUSE_FIRST + 1
};
//===================================================================================================
enum class tf_roundstates : int
{
	STATE_INIT = 0,       // Initialize the game, create teams.
	STATE_PREGAME,       // Before players have joined the game.
	STATE_STARTGAME,   // Game is about to start.
	STATE_PREROUND,    //All players are respawned.
	STATE_RND_RUNNING, // Playing normally.
	STATE_TEAM_WIN,     // Someone has won the round.
	STATE_RESTART,        // Manually restart the game, reset scores.
	STATE_STALEMATE,   // Nobody has won, restart game.
	STATE_GAME_OVER  // Game is over, showing the scoreboard, etc.
};
//===================================================================================================
enum class tf_hitbox : int
{
	head = 0,
	pelvis = 1,
	spine_0 = 2,
	spine_1 = 3,
	spine_2 = 4,
	spine_3 = 5,
	upperArm_L = 6,
	lowerArm_L = 7,
	hand_L = 8,
	upperArm_R = 9,
	lowerArm_R = 10,
	hand_R = 11,
	hip_L = 12,
	knee_L = 13,
	foot_L = 14,
	hip_R = 15,
	knee_R = 16,
	foot_R = 17,
};