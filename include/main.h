#ifndef MAIN_H
#define MAIN_H
#include <common.h>
#include <colors.h>
#include <hashcodes.h>
#include <Sound.h>
#include <textprint.h>
#include <buttons.h>
#include <playerstate.h>
#include <symbols.h>
#include <screenmath.h>

extern bool MOD_INIT;

extern int gMap_MechaRed;
extern int gLevel_VolcanoDescent2;

//Temporary raycast memory
typedef struct RayVecs {
    EXVector vecs[3];
} RayVecs;

typedef enum DeathMode {
    ReloadGame,
    PlayerDespawn,
    DEATHMODE_AMOUNT
} DeathMode;

extern DeathMode deathMode;

//Empty color


//VTABLES

extern int SPYRO_VTABLE;
extern int HUNTER_VTABLE;
extern int BLINK_VTABLE;
extern int SGTBYRD_VTABLE;
extern int SPARX_PLAYER_VTABLE;
extern int BALLGADGET_VTABLE;
extern int EMBER_VTABLE;
extern int FLAME_VTABLE;

extern int SPARX_VTABLE;
extern int CAMERA_FOLLOW_VTABLE;
extern int CAMERA_BALLGADGET_FOLLOW_VTABLE;
extern int CAMERA_BOSS_VTABLE;
extern int BLINKYBULLET_VTABLE;
extern int LOCKEDCHEST_VTABLE;
extern int BOSS_VTABLE;

//Lookup tables for characters and their vtables

#define CHARACTER_AMOUNT 7
extern Players CHARACTERS[];
extern int CHARACTER_VTABLES[];

//Whether players have been initialized yet (mostly for testing reasons)
extern bool initialized;
//Whether to display debug information on the screen
extern bool showDebug;
//How many frames the "show debug button" has been held
extern int showDebugTimer;

extern bool doMultiplayerOptions;

extern bool showMP_Notifs;

//Whether a notification is being shown for the given player
extern bool playerNotifShowing[];

//Y-positions of the player notifications on the left of the screen
extern u16 playerNotifYOffets[];

//Notif timer for when a player joins
extern int playerJoinedNotifTimers[];

//Notif timer for when a player isn't loaded and has to wait before joining
extern int notLoadedYetNotifTimers[];

//How many frames the "restart game" button has been held
extern int restartGameTimer;
//How many frames held before restarting
extern int restartGameTimerMax;

//Notif timer for when a player selects a new breath
extern int breathSelectNotifTimers[];
//This is set when any player selects a new breath
extern Breaths lastBreathChangedTo;

//The custom item that the follower camera is set to target
extern int* cameraTargetItem;

//Last player that was updated
extern int lastPlayerUpdated;
//Controller port number of the global player references. Should be made to match gpPlayer and gpPlayerItem.
extern int globalRefPortNr;

//How many frames each player has held down the button to restore control/visibility
extern int playerRestoreTimers[4];
//How many frames held before restoring
extern int playerRestoreTimerMax;

//Notif timer for when a player is restoring control
extern int playerRestoreNotifTimers[];

//How many frames each player has held down the button to leave
extern int playerLeaveTimers[4];
//How many frames held before leaving
extern int playerLeaveTimerMax;
extern bool leaveBecauseDeath[];

//Notif timer for when a player is leaving
extern int playerLeaveNotifTimers[];

//How many frames left for each player until they can join
extern int playerJoinCooldownTimers[];
//The value the cooldown timers are set to when a player dies
#define PLAYER_JOIN_COOLDOWN_MAX_DEFAULT 60 * 10 //20 seconds
extern int playerJoinCooldownTimerMax;

extern bool showCoolDownTimer;

//Current player handler being tracked for doing a breath attack
extern int* currentBreather;

//Functions

void SetupVtableHooks();

//Check for the Z button being pressed twice within 20 frames at the given pad number
bool checkZDoublePress(int padNr);

//Distance between two EXVectors
float EXVector_Dist(EXVector* v1, EXVector* v2);

//Copy EXVector src into dest
void EXVector_Copy(EXVector* dest, EXVector* src);

//Get the runtime class from the given handler
EXRuntimeClass* GetHandlerRuntimeClass(int* handler);

//Check if the given handler is an instance of or inherits from the given runtime class
bool HandlerIsOrInheritsFrom(int* handler, EXRuntimeClass* class);

//Get the flag for the gameloop being paused
bool GameIsPaused();

//Get a string representation of the given breath
char* GetBreathName(Breaths breath);

//Get the color associated with the given health value
XRGBA* GetHealthColor(int health);

//Get the function pointer to the PlayerSetup method for the given map
SE_Map_v_PlayerSetup GetMapPlayerSetupFunc(int* map);

//Get the function pointer to the Delete method for the given player handler
ItemHandler_Delete GetHandlerDeleteFunc(int* handler);

//Checks if the given item is found in the item list
bool itemExists(int* item);

//Create camera target item if it doesn't exist or if it's uninitialized
void updateCameraTargetItem();

//Code that runs prior to a player's SEUpdate.
//Apply hotswap values if players have joined.
void PlayerHandlerPreUpdate(int* self);

//Code that runs after a player's SEUpdate.
//Store resulting hotswap values for the player.
void PlayerHandlerPostUpdate(int* self);

//Code that runs before Sparx updates.
//Follow the first available player.
//Make Sparx react to the lowest health among players.
void SparxPreUpdate(int* self);

//Code that runs before any other handler updates.
//Set global references to the closest player.
void MiscHandlerPreUpdate(int* self);

void CameraBallGadgetFollowPostUpdate(int* self);

//Code that runs after the follower camera's SEUpdate.
//Manipulate camera variables to make them work with multiple players.
void CameraFollowPostUpdate(int* self);

void CameraBossPostUpdate(int* self);

//Draw the player's name, health and powerup status
void DrawPlayerMarker(int portNr);

void DrawMultiplayerMenu();

void HandleMultiplayerMenu();

//main_hook.s | Runs every frame
void MainUpdate();

//draw_hook.s | Runs every frame during the HUD draw loop
//Drawing stuff to the screen should be done here to avoid garbled textures
void DrawUpdate();

int GUI_PauseMenu_v_DrawStateRunning_Hook(int* self, int* pWnd);

//scan_hook.s | On the frame this function returns true, scanmode will be enabled/disabled.
//Typically you'd return true if a button has just been pressed.
bool ScanUpdate();

//runs right before an itemhandler is updated
bool ItemHandler_SEUpdate_Hook(int* self);

//Replaces the call to EXItemEnv::UpdateItems_Physics
//replaces the original code but adds swapping around player references.
void EXItemEnv_UpdateItems_Physics_Hook(int* self);

//Runs before the game checks for the player changing breath
//Only used for the "player changed breath" notification on the HUD
bool TestBreathChangeHook(int* self);

//After sparx eats a butterfly (add 1 unit of health to all players)
void Sparx_SetPlayerHealth_Hook(int* self, int health);

//After a special butterfly is collected (set all health to full)
void Butterfly_Special_SetHealth_Hook(int* self, int health);

//Returns whether or not a map inherets from or is an instance of SEMap_MiniGame
bool MapIsMinigame(int* map);

bool VtableSwap_Player_TestBreathType(int* self, BreathType type);

//Replaces function at 0x800a31b0 and 0x80047f4c (both Spyro and Hunter die the same way)
void ReImpl_SpyroHunter_urghhhImDead(int* self);

//Replaces function at 0x80020ebc
void ReImpl_Blink_urghhhImDead(int* self);

//Replaces function at 0x8007c440
void ReImpl_SgtByrd_urghhhImDead(int* self);

void GadgetPad_SuperCharge_Hook();

void GadgetPad_Invincibility_Hook();

//Edit behavior of icons when running in multiplayer
int GUI_PanelItem_v_StateRunning_Hook(int* self);

//unused hook
bool XGameWnd_Draw_Hook(int* self);

//Replaces the Delete method for the XSEItemHandler_Player class
int ReImpl_XSEItemHandler_Player_Delete(int* self);

//Replaces the DoKill method call on the XSEItemHandler class
//when called from one of the players' Delete method when it's trying to delete Sparx
void ReImpl_XSEItemHandler_DoKill(int* self);

void ChangePlayer_Hook();

bool Particle_Fire_FlameObjects_Hook(int* self, int breath, EXVector* pos, float dist);

#endif