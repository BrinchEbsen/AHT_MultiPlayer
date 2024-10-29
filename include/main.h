#ifndef MAIN_H
#define MAIN_H
#include <common.h>
#include <hashcodes.h>
#include <Sound.h>
#include <textprint.h>
#include <buttons.h>
#include <playerstate.h>
#include <symbols.h>
#include <screenmath.h>

bool MOD_INIT = false;

int gMap_MechaRed = 0x8045b5d8;
int gLevel_VolcanoDescent2 = 0x8045e480;

//Temporary raycast memory
struct RayVecs {
    EXVector vecs[3];
};
typedef struct RayVecs RayVecs;

enum DeathMode {
    ReloadGame,
    PlayerDespawn,
    DEATHMODE_AMOUNT
};
typedef enum DeathMode DeathMode;

DeathMode deathMode = PlayerDespawn;

//Empty color
XRGBA COLOR_BLANK = {0, 0, 0, 0};

XRGBA COLOR_P1 = {0x00, 0x48, 0x80, 0x80}; //Blue
XRGBA COLOR_P2 = {0x80, 0x20, 0x20, 0x80}; //Red
XRGBA COLOR_P3 = {0x08, 0x60, 0x00, 0x80}; //Green
XRGBA COLOR_P4 = {0x80, 0x50, 0x00, 0x80}; //Yellow

XRGBA COLOR_INV = {0x40, 0x40, 0x80, 0x80};
XRGBA COLOR_SUP = {0x80, 0x40, 0x40, 0x80};

XRGBA* PLAYER_COLORS[] = {
    &COLOR_P1,
    &COLOR_P2,
    &COLOR_P3,
    &COLOR_P4
};

//Colors of health when the hitpoint upgrade isn't purchased
XRGBA* HEALTH_COLORS_NO_UPGRADE[] = {
    /*0x00*/ &COLOR_BLACK,
    /*0x20*/ &COLOR_BLACK,
    /*0x40*/ &COLOR_BLACK,
    /*0x60*/ &COLOR_GREEN,
    /*0x80*/ &COLOR_BLUE,
    /*0xA0*/ &COLOR_P4
};

//Colors of health when the hitpoint upgrade is purchased
XRGBA* HEALTH_COLORS_UPGRADE[] = {
    /*0x00*/ &COLOR_BLACK,
    /*0x20*/ &COLOR_BLACK,
    /*0x40*/ &COLOR_RED,
    /*0x60*/ &COLOR_GREEN,
    /*0x80*/ &COLOR_BLUE,
    /*0xA0*/ &COLOR_P4
};

//Hotswap values for temporary raycast memory
RayVecs PLAYER_RAYVECS[4];

//The names that are displayed above the players
char* PLAYER_NAMES[] = { "P1", "P2", "P3", "P4" };

//Hotswap values for selected breath
Breaths PLAYER_BREATHS[] = {
    Breath_Fire,
    Breath_Fire,
    Breath_Fire,
    Breath_Fire
};

//Hotswap values for health
int PLAYER_HEALTH[] = { 0xA0, 0xA0, 0xA0, 0xA0 };

//Hotswap values for whether supercharge is active
bool PLAYER_SUPERCHARGE[] = {false, false, false, false};
//Hotswap values for supercharge timers
float PLAYER_SUPERCHARGE_TIMER[] = { 0.0, 0.0, 0.0, 0.0};

//Hotswap values for whether invincibility is active
bool PLAYER_INVINCIBILITY[] = {false, false, false, false};
//Hotswap values for invincibility timers
float PLAYER_INVINCIBILITY_TIMER[] = { 0.0, 0.0, 0.0, 0.0};

//VTABLES

int SPYRO_VTABLE = 0x8040B908;
int HUNTER_VTABLE = 0x80407338;
int BLINK_VTABLE = 0x80406488;
int SGTBYRD_VTABLE = 0x8040aea8;
int SPARX_PLAYER_VTABLE = 0x80405e38;
int BALLGADGET_VTABLE = 0x80404ec0;
int EMBER_VTABLE = 0x804051d8;
int FLAME_VTABLE = 0x80405428;

int SPARX_VTABLE = 0x8040f8b0;
int CAMERA_FOLLOW_VTABLE = 0x804161b0;
int CAMERA_BALLGADGET_FOLLOW_VTABLE = 0x80415790;
int CAMERA_BOSS_VTABLE = 0x80415650;
int BLINKYBULLET_VTABLE = 0x80408988;
int LOCKEDCHEST_VTABLE = 0x80429b18;
int BOSS_VTABLE = 0x80407148;

//Lookup tables for characters and their vtables

#define CHARACTER_AMOUNT 7
Players CHARACTERS[] = {
    Player_Spyro,
    Player_Hunter,
    Player_Blinky,
    Player_SgtByrd,
    //Player_Sparx,
    Player_BallGadget,
    Player_Ember,
    Player_Flame
};
int CHARACTER_VTABLES[] = {
    0x8040B908, //Spyro
    0x80407338, //Hunter
    0x80406488, //Blink
    0x8040aea8, //Sgt. Byrd
    //0x80405e38, //Sparx
    0x80404ec0, //Ball Gadget
    0x804051d8, //Ember
    0x80405428  //Flame
};

//Whether players have been initialized yet (mostly for testing reasons)
bool initialized = false;
//Whether to display debug information on the screen
bool showDebug = false;
//How many frames the "show debug button" has been held
int showDebugTimer = 0;

bool doMultiplayerOptions = false;

//Whether a notification is being shown for the given player
bool playerNotifShowing[] = {
    false,
    false,
    false,
    false
};

//Y-positions of the player notifications on the left of the screen
u16 playerNotifYOffets[] = {
    250,
    265,
    280,
    295
};

//Notif timer for when a player joins
int playerJoinedNotifTimers[] = {0, 0, 0, 0};

//Notif timer for when a player isn't loaded and has to wait before joining
int notLoadedYetNotifTimers[] = {0, 0, 0, 0};

//How many frames the "restart game" button has been held
int restartGameTimer = 0;
//How many frames held before restarting
int restartGameTimerMax = 80;

//Notif timer for when a player selects a new breath
int breathSelectNotifTimers[] = {0, 0, 0, 0};
//This is set when any player selects a new breath
Breaths lastBreathChangedTo = Breath_Fire;

//The custom item that the follower camera is set to target
int* cameraTargetItem = NULL;

//Contains the itemhandler ID's for each of the four players. -1 if not in use.
int players[4] = {-1, -1, -1, -1};
//Last player that was updated
int lastPlayerUpdated = 0;
//Controller port number of the global player references. Should be made to match gpPlayer and gpPlayerItem.
int globalRefPortNr = 0;

//How many frames each player has held down the button to restore control/visibility
int playerRestoreTimers[4] = {0, 0, 0, 0};
//How many frames held before restoring
int playerRestoreTimerMax = 60;

//Notif timer for when a player is restoring control
int playerRestoreNotifTimers[] = {0, 0, 0, 0};

//How many frames each player has held down the button to leave
int playerLeaveTimers[4] = {0, 0, 0, 0};
//How many frames held before leaving
int playerLeaveTimerMax = 90;
bool leaveBecauseDeath[] = {false, false, false, false};

//Notif timer for when a player is leaving
int playerLeaveNotifTimers[] = {0, 0, 0, 0};

//How many frames left for each player until they can join
int playerJoinCooldownTimers[] = {0, 0, 0, 0};
//The value the cooldown timers are set to when a player dies
#define PLAYER_JOIN_COOLDOWN_MAX_DEFAULT 60 * 10 //20 seconds
int playerJoinCooldownTimerMax = PLAYER_JOIN_COOLDOWN_MAX_DEFAULT;

bool showCoolDownTimer = false;

//Current player handler being tracked for doing a breath attack
int* currentBreather = NULL;

//Functions

void SetupVtableHooks();

//Check for the Z button being pressed twice within 20 frames at the given pad number
bool checkZDoublePress(int padNr);

//Approximate luminance from RGB
int XRGBA_Luminance(XRGBA* col);

//Balance RGB values to match given luminance
void XRGBA_Balance(XRGBA* col, int bal);

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

//Get the number of player items currently kept track of
int NumberOfPlayers();

//Get the number of player items currently kept track of, who can have Sparx following around
int NumberOfPlayersWhoCanHaveSparx();

//Check if the only player is player 1
bool OnlyPlayer1Exists();

//Check if the given handler is the only player left.
bool handlerIsOnlyPlayerLeft(int* handler);

//Get the associated controller port index for the given player handler
int GetPortNrFromPlayerHandler(int* handler);

//Set the global port number, handler reference and handler item reference to the player at the given port number.
//Returns whether the player handler could be found.
bool SetPlayerRefToPort(int portNr);

//Check if the given handler's vtable matches that of a player
bool HandlerIsPlayer(int* handler);

//Runs through all players, inserts the handlers into the given list of size 4, and returns amount of handlers found.
int GetArrayOfPlayerHandlers(int** list);

//Get the first player who has sparx.
int* GetFirstPlayerNotZeroHP();

//Reset all player references and search the item list to populate it again.
void initializePlayers();

//Remove any handler references that don't exist anymore.
//If no players are referenced, do a full initialize of the player list.
void updatePlayerList();

//Add new player and assign it to the given port number
void addNewPlayer(int portNr);

//Remove a player from the game, restore follower camera, reload game if this was the last player left
void removePlayer(int portNr, bool died);

//Set a player to be visible and controllable
void restorePlayerControl(int portNr);

//Teleport all players to the player described by portNr
void teleportPlayersToPlayer(int portNr);

//Returns whatever pad number is moving the right stick the most
int whoShouldControlCamera();

//Checks if the given item is found in the item list
bool itemExists(int* item);

//Create camera target item if it doesn't exist or if it's uninitialized
void updateCameraTargetItem();

//Get the middle point between all players, as well as the biggest range between them.
//Returns false if no players were found.
//Tries ignoring any player in the dying state.
bool GetPlayerPosMidAndRanges(EXVector3* middle, float* biggestRange);

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

//Sets all players' health and the global health to full
void SetAllHealthFull();

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