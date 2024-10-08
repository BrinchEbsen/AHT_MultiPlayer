#include <common.h>
#include <hashcodes.h>
#include <Sound.h>
#include <textprint.h>
#include <buttons.h>
#include <playerstate.h>
#include <symbols.h>
#include <screenmath.h>

#define PI 3.14159265359

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

//Vtable hook stuff

int GUI_PanelItem_v_StateRunning_Hook(int* self);
GUI_PanelItem_v_StateRunning_func GUI_PanelItem_v_StateRunning_hookFunc = GUI_PanelItem_v_StateRunning_Hook;

int GUI_PauseMenu_v_DrawStateRunning_Hook(int* self, int* pWnd);
GUI_PauseMenu_v_DrawStateRunning_func GUI_PauseMenu_v_DrawStateRunning_hookFunc = GUI_PauseMenu_v_DrawStateRunning_Hook;

void LoadingLoopDraw_ReImpl(int* self, int* pWnd);
LoadingLoopDraw_func LoadingLoopDraw_hookFunc = LoadingLoopDraw_ReImpl;

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

//Check for the Z button being pressed twice within 20 frames at the given pad number
bool checkZDoublePress(int padNr) {
    //Number of presses within the timer window
    static int numPressed[] = {0, 0, 0, 0};
    //Number of frames since first press
    static int timer[] = {0, 0, 0, 0};

    bool zPressed = isButtonPressed(Button_Z, padNr);

    //If Z has not been pressed now or before, reset timer and return
    if (numPressed[padNr] == 0 && !zPressed) {
        timer[padNr] = 0;

        return false;
    }

    //Have either pressed it before or am pressing now, so increase timer
    timer[padNr]++;

    //If timer is within the limit
    if (timer[padNr] <= 20) {
        if (zPressed) { numPressed[padNr]++; }

        if (numPressed[padNr] >= 2) {
            //Have successfully double-pressed
            timer[padNr] = 0;
            numPressed[padNr] = 0;

            return true;
        }

        return false;
    } else {
        //Timer expired
        timer[padNr] = 0;
        numPressed[padNr] = 0;
    
        return false;
    }

    //Just for safety
    return false;
}

//Approximate luminance from RGB
int XRGBA_Luminance(XRGBA* col) {
    int r = col->r;
    int g = col->g;
    int b = col->b;

    return (r+r+r+b+g+g+g+g)>>3;
}

//Balance RGB values to match given luminance
void XRGBA_Balance(XRGBA* col, int bal) {
    int lum = XRGBA_Luminance(col);
    float ratio = (float)bal/(float)lum;

    col->r = (int)(col->r * ratio);
    col->g = (int)(col->g * ratio);
    col->b = (int)(col->b * ratio);
}

//Distance between two EXVectors
float EXVector_Dist(EXVector* v1, EXVector* v2) {
    EXVector3 mag = {
        .x = v2->x - v1->x,
        .y = v2->y - v1->y,
        .z = v2->z - v1->z
    };

    return ig_sqrtf(
        (mag.x * mag.x) + 
        (mag.y * mag.y) + 
        (mag.z * mag.z)
    );
}

//Copy EXVector src into dest
void EXVector_Copy(EXVector* dest, EXVector* src) {
    dest->x = src->x;
    dest->y = src->y;
    dest->z = src->z;
    dest->w = src->w;
}

//Get the runtime class from the given handler
EXRuntimeClass* GetHandlerRuntimeClass(int* handler) {
    int* vtable = (int*) *(handler + (0x4/4));
    GetRuntimeClass_func getRTC = (GetRuntimeClass_func) *(vtable + (0x8/4));
    return getRTC();
}

//Check if the given handler is an instance of or inherits from the given runtime class
bool HandlerIsOrInheritsFrom(int* handler, EXRuntimeClass* class) {
    EXRuntimeClass* myClass = GetHandlerRuntimeClass(handler);

    while (class != NULL) {
        if (myClass == class) {
            return true;
        }

        class = class->pBaseClass;
    }

    return false;
}

//Get the flag for the gameloop being paused
bool GameIsPaused() {
    int flags = *((&gGameLoop) + (0x88/4));
    return (flags & 0x80000000) != 0;
}

//Get a string representation of the given breath
char* GetBreathName(Breaths breath) {
    switch (breath) {
        case Breath_Fire:
            return "Fire";
        case Breath_Electric:
            return "Electric";
        case Breath_Water:
            return "Water"; 
        case Breath_Ice:
            return "Ice";
    }

    return "Invalid";
}

//Get the color associated with the given health value
XRGBA* GetHealthColor(int health) {
    //Sparx' colors are different depending on if the upgrade has been bought
    bool gottenUpgrade = (gPlayerState.AbilityFlags & Abi_HitPointUpgrade) != 0;

    //Each unit of health is 0x20
    int index = health/0x20;

    if (gottenUpgrade) {
        return HEALTH_COLORS_UPGRADE[index];
    } else {
        return HEALTH_COLORS_NO_UPGRADE[index];
    }
}

//Get the function pointer to the PlayerSetup method for the given map
SE_Map_v_PlayerSetup GetMapPlayerSetupFunc(int* map) {
    int* vtable = (int*) *(map + (0x74/4));

    return *(vtable + (0xc0/4));
}

//Get the function pointer to the Delete method for the given player handler
ItemHandler_Delete GetHandlerDeleteFunc(int* handler) {
    int* vtable = (int*) *(handler + (0x4/4));

    return *(vtable + (0x50/4));
}

//Get the number of player items currently kept track of
int NumberOfPlayers() {
    int count = 0;

    for (int i = 0; i < 4; i++) {
        if (players[i] != -1) {
            count++;
        }
    }

    return count;
}

//Get the number of player items currently kept track of, who can have Sparx following around
int NumberOfPlayersWhoCanHaveSparx() {
    int count = 0;

    for (int i = 0; i < 4; i++) {
        if (players[i] != -1) {
            int* handler = ItemEnv_FindUniqueIDHandler(&theItemEnv, players[i], 0);
            if (handler == NULL) { continue; }

            int vtable = *(handler + 0x4/4);

            //These characters cannot have sparx (bizarrely Sgt. Byrd isn't included)
            if (
                (vtable == BALLGADGET_VTABLE) ||
                (vtable == SPARX_PLAYER_VTABLE)
            ) {
                continue;
            }

            count++;
        }
    }

    return count;
}

//Check if the only player is player 1
bool OnlyPlayer1Exists() {
    return (players[0] != -1) && (NumberOfPlayers() == 1);
}

//Check if the given handler is the only player left.
bool handlerIsOnlyPlayerLeft(int* handler) {
    int ID = *(handler + (0x8/4));

    for (int i = 0; i < 4; i++) {
        if (players[i] == ID) { continue; } //ignore self

        if (players[i] != -1) { return false; }
    }

    return true;
}

//Get the associated controller port index for the given player handler
int GetPortNrFromPlayerHandler(int* handler) {
    if (handler == NULL) { return 0; }

    int ID = *(handler + 0x8/4);
    for (int i = 0; i < 4; i++) {
        if (players[i] == ID) {
            return i;
        }
    }

    return 0;
}

//Set the global port number, handler reference and handler item reference to the player at the given port number.
//Returns whether the player handler could be found.
bool SetPlayerRefToPort(int portNr) {
    int* handler = ItemEnv_FindUniqueIDHandler(&theItemEnv, players[portNr], 0);
    if (handler == NULL) { return false; }

    g_PadNum = portNr;
    gpPlayer = handler;
    gpPlayerItem = (int*) *handler;
    globalRefPortNr = portNr;

    return true;
}

//Check if the given handler's vtable matches that of a player
bool HandlerIsPlayer(int* handler) {
    if (handler == NULL) { return false; }
    int vtable = *(handler + 0x4/4);

    for (int j = 0; j < CHARACTER_AMOUNT; j++) {
        if (CHARACTER_VTABLES[j] == vtable) {
            return true;
        }
    }

    return false;
}

//Runs through all players, inserts the handlers into the given list of size 4, and returns amount of handlers found.
int GetArrayOfPlayerHandlers(int** list) {
    int listindex = 0;
    
    for (int i = 0; i < 4; i++) {
        if (players[i] == -1) { continue; }

        int* handler = ItemEnv_FindUniqueIDHandler(&theItemEnv, players[i], 0);
        if (handler == NULL) { continue; }

        list[listindex] = handler;
        listindex++;
    }

    //Size of list
    return listindex;
}

int* GetFirstPlayerNotZeroHP() {
    int portNr = 0;

    for (int i = 0; i < 4; i++) {
        if (players[i] != -1) {
            portNr = i;
        } else {
            continue;
        }

        bool upgrade = (gPlayerState.AbilityFlags & Abi_HitPointUpgrade) != 0;
        int health = PLAYER_HEALTH[i];

        bool death = false;
        if (upgrade) {
            death = health <= 0x0;
        } else {
            death = health <= 0x20;
        }

        if (!death) {
            portNr = i;
            break;
        }
    }

    return ItemEnv_FindUniqueIDHandler(&theItemEnv, players[portNr], 0);
}

//Reset all player references and search the item list to populate it again.
void initializePlayers() {
    //Reset references
    players[0] = -1;
    players[1] = -1;
    players[2] = -1;
    players[3] = -1;

    //Loop through item list
    if (ItemEnv_ItemCount != 0) {
        EXDListItem* item = ItemEnv_ItemList->head;

        int playerIndex = 0;
        while (item != NULL) {
            //Get handler
            int* handler = (int*) *((int*)item + (0x14C/4));
            
            //If the item is that of a player, insert it into our references list
            if (HandlerIsPlayer(handler)) {
                int ID = *(handler + 0x8/4);
                players[playerIndex] = ID;
                playerIndex++;
                if (playerIndex > 3) {break;}
            }

            item = item->next;
        }
    }

    initialized = true;
}

//Remove any handler references that don't exist anymore.
//If no players are referenced, do a full initialize of the player list.
void updatePlayerList() {
    bool allEmpty = true;
    
    for (int i = 0; i < 4; i++) {
        if (players[i] != -1) {
            allEmpty = false;

            int* handler = ItemEnv_FindUniqueIDHandler(&theItemEnv, players[i], 0);

            if (handler == NULL) {
                //This handler has unloaded, reset reference
                players[i] = -1;
            }
        }
    }

    if (allEmpty) {
        //No players were found, do a full search of the item list to find some
        initializePlayers();
    }
}

bool modeIsDying(PlayerModes mode) {
    switch (mode) {
        case death:
        case swamp_death:
        case water_death:
        case deathfall:
        case iceydeath:
            return true;
    }

    return false;
}

void reinitializeCamera(int* player) {
    if (player == NULL) { return; }
    int* cameraHandler = (int*) *(gpGameWnd + (0x378/4));

    if (cameraHandler != NULL) {
        int* vtable = (int*) *(cameraHandler + (0x4/4));
        GetRuntimeClass_func getRTC = (GetRuntimeClass_func) *(vtable + (0x8/4));

        EXRuntimeClass* class = getRTC();

        //If it isn't a follower camera, set it to a new follower camera instance
        if (class != 0x803bf64c) { //XSEItemHandler_Camera_Follow::classXItemHandler_Camera_Follow
            int* playerItem = (int*) *player;
            Players playerType = (Players) *(player + (0x578/4));

            int* target = NULL;
            if (cameraTargetItem == NULL) {
                target = cameraTargetItem;
            } else {
                target = playerItem;
            }

            SetCamera(Follow, ForcePos, target, playerType, 0);
        } else {
            int* flags = cameraHandler + (0x39c/4);
            *flags = 0; //If it is already a follower camera, just reset the flags
        }
    }
}

//Add new player and assign it to the given port number
void addNewPlayer(int portNr) {
    //Only perform on players 2, 3 and 4
    if ((portNr < 0) || (portNr > 3)) { return; }

    if (players[portNr] != -1) { return; } //Player already exists here

    //Get current map
    int* map = GetSpyroMap(0);
    if (map == NULL) { return; }

    //Find the first available player that already exists (this is where we spawn the new player next to)
    int* list[4];
    int count = GetArrayOfPlayerHandlers(&list);
    if (count == 0) { return; } //there are no other players, the game should've restarted long ago

    int* handler = list[0];

    //Get item
    int* item = (int*)(*handler);

    //New player is spawned above and to the side of this player
    EXVector* pos = (EXVector*) (item + (0xD0/4));
    EXVector v = {
        pos->x + 2.0,
        pos->y + 3.0,
        pos->z,
        pos->w
    };

    //Get rotation
    EXVector* rot = (EXVector*) (item + (0xE0/4));

    int* newPlayer;

    //Special case for minecart section due to the special path physics
    if (map == gLevel_VolcanoDescent2) {
        SEMap_BallGadget_AddPlayer(map, pos, rot);
        newPlayer = gpPlayer;
    } else {
        //Get the player setup function from the map's vtable
        SE_Map_v_PlayerSetup setupFunc = GetMapPlayerSetupFunc(map);

        setupFunc(map, &v, rot);

        //get the "player" field in the global PlayerSetup
        Players* setup_Player = &gPlayerState.Setup.Player;
        Players player1_Player = (Players) *(handler + (0x578/4));

        *setup_Player = player1_Player;

        //TODO: Let player spawn as the character they want

        //Ensure the player is loaded/loading
        PlayerLoader_PreLoad(&gPlayerLoader, *setup_Player);
        //Check if it's loaded, set a notification and abort if not.
        if (!PlayerLoader_IsLoaded(&gPlayerLoader, *setup_Player)) {
            notLoadedYetNotifTimers[portNr] = 60;
            return;
        } else {
            notLoadedYetNotifTimers[portNr] = 0;
        }

        //If the player is set to be Spyro, make it Ember for port 2 and Flame for port 3
        if ((*setup_Player == Player_Spyro) || (*setup_Player == Player_Ember) || (*setup_Player == Player_Flame)) {
            if (portNr == 1) {
                *setup_Player = Player_Ember;
            } else if (portNr == 2) {
                *setup_Player = Player_Flame;
            } else {
                *setup_Player = Player_Spyro;
            }
        }

        //Create new player handler
        newPlayer = CreatePlayer(map);
    }
    
    if (newPlayer == NULL) { return; }

    //Grab ID and assign it to the player
    int ID = *(newPlayer + 0x8/4);
    players[portNr] = ID;

    //Finally set some notification values
    playerJoinedNotifTimers[portNr] = 60;
    playerLeaveNotifTimers[portNr] = 0;
}

//Remove a player from the game, restore follower camera, reload game if this was the last player left
void removePlayer(int portNr, bool died) {
    //Only perform on players 1, 2, 3 and 4
    if ((portNr < 0) || (portNr > 3)) { return; }

    //We really don't care what player it is, if it's the last one, so just restart the game if that's the case
    if (NumberOfPlayers() <= 1) {
        SetAllHealthFull();
        PlayerState_RestartGame(&gPlayerState);
        return;
    }

    if (players[portNr] == -1) { return; }
    
    int* handler = ItemEnv_FindUniqueIDHandler(&theItemEnv, players[portNr], 0);
    if (handler == NULL) { return; }

    //let's now delete the thing
    gpPlayer = handler; //global references are set first to avoid crashes
    gpPlayerItem = (int*) *handler;
    ItemHandler_SEKill(handler);
    players[portNr] = -1;

    //Set the global references to the first available player
    int* list[4];
    int count = GetArrayOfPlayerHandlers(&list);
    if (count != 0) {
        int* newHandler = list[0];
        gpPlayer = newHandler;
        gpPlayerItem = (int*) *newHandler;
        globalRefPortNr = GetPortNrFromPlayerHandler(newHandler);
    }

    //if (gpSparx != NULL) {
    //    Sparx_SetHealthState(gpSparx);
    //    
    //    Sparx_HandleHiding(gpSparx, 1);
    //    Sparx_SetMode(gpSparx, spx_chasingFodder, 0);
    //}

    //Finally, set some notification values
    playerJoinedNotifTimers[portNr] = 0;
    playerLeaveNotifTimers[portNr] = 60;
    leaveBecauseDeath[portNr] = died;
}

//Set a player to be visible and controllable
void restorePlayerControl(int portNr) {
    if (portNr == -1) { return; }

    int* handler = ItemEnv_FindUniqueIDHandler(&theItemEnv, players[portNr], 0);
    if (handler == NULL) { return; }

    //Set visible and unlock controls
    Player_SetVisibility(handler, true);
    Player_UnlockControls(handler);

    //If the current mode is "cutscene", set it to idle instead.
    PlayerModes mode = *(handler + (0x834/4));
    if ((mode == listen) || (mode == listen_water)) {
        Player_SetMode(handler, 1, 0, 0);
    }
}

//Teleport all players to the player described by portNr
void teleportPlayersToPlayer(int portNr) {
    if (players[portNr] == -1) { return; }

    int* handler = ItemEnv_FindUniqueIDHandler(&theItemEnv, players[portNr], 0);
    if (handler == NULL) { return; }

    int* list[4];
    int count = GetArrayOfPlayerHandlers(&list);
    if (count < 2) { return; } //Ignore if only 1 player

    int* item = (int*) *handler;
    EXVector* pos = (EXVector*) (item + (0xD0/4));

    for (int i = 0; i < count; i++) {
        if (i == portNr) { continue; } //ignore self

        int* oHandler = list[i];
        int* oItem = (int*) *oHandler;

        EXVector* oPos = (EXVector*) (oItem + (0xD0/4));

        oPos->x = pos->x;
        oPos->y = pos->y;
        oPos->z = pos->z;
    }
}

//Returns whatever pad number is moving the right stick the most
int whoShouldControlCamera() {
    int port = 0;
    float biggestMag = 0.0;
    bool first = true;

    for (int i = 0; i < 4; i++) {
        if (players[i] == -1) { continue; }

        float x = ig_fabsf(Pads_Analog[i].RStick_X);
        float y = ig_fabsf(Pads_Analog[i].RStick_Y);
        float mag = ig_sqrtf((x*x) + (y*y));

        if (first) {
            port = i;
            biggestMag = mag;
            first = false;
            continue;
        }

        if (biggestMag < mag) {
            port = i;
            biggestMag = mag;
        }
    }

    return port;
}

//Checks if the given item is found in the item list
bool itemExists(int* item) {
    if (item == NULL) { return false; }

    if (ItemEnv_ItemCount == 0) { return false; }

    EXDListItem* i = ItemEnv_ItemList->head;

    while (i != NULL) {
        if ((int*)i == item) {
            return true;
        }

        i = i->next;
    }

    return false;
}

//Create camera target item if it doesn't exist or if it's uninitialized
void updateCameraTargetItem() {
    if (!itemExists(cameraTargetItem)) {
        //Construct camera target item
        cameraTargetItem = XSEItem_CreateObject();
        //add to iten environment
        XSEItemEnv_AddXSEItem(&theItemEnv, cameraTargetItem, 0);
        //ig_printf("Creating new item for camera focus\n");
    }
}

//Get the middle point between all players, as well as the biggest range between them.
//Returns false if no players were found.
//Tries ignoring any player in the dying state.
bool GetPlayerPosMidAndRanges(EXVector3* middle, float* biggestRange) {
    int* list[4];
    int count = GetArrayOfPlayerHandlers(&list);

    if (count == 0) { return false; }

    //Lower and upper ranges for positions the players take up
    EXVector3 rangeLower = {0.0};
    EXVector3 rangeUpper = {0.0};

    bool init = false;

    //Loop through all players and gather the ranges of their positions
    for (int i = 0; i < count; i++) {
        int* handler = list[i];

        int* playerItem = (int*) *handler;
        EXVector* playerPos = (EXVector*) (playerItem + (0xD0/4));

        //Ignore if the player is dying (to avoid annoying camera angle).
        //Don't ignore if we haven't initialized, otherwise we have no initial vectors for the focus.
        //Also don't ignore if we're at the end of the list, because no player afterwards will be able to initialize the vectors.
        if (init || (i != (count-1))) {
            PlayerModes mode = (PlayerModes) *(handler + (0x834/4));
            if (modeIsDying(mode)) {
                continue;
            }
        }

        //Initialize the range vectors for the first player
        if (!init) {
            init = true;

            rangeLower.x = playerPos->x;
            rangeLower.y = playerPos->y;
            rangeLower.z = playerPos->z;
            rangeUpper.x = playerPos->x;
            rangeUpper.y = playerPos->y;
            rangeUpper.z = playerPos->z;
            continue;
        }

        //Update ranges for the subsequent players
        if (playerPos->x > rangeUpper.x) {
            rangeUpper.x = playerPos->x;
        } else if (playerPos->x < rangeLower.x) {
            rangeLower.x = playerPos->x;
        }
        if (playerPos->y > rangeUpper.y) {
            rangeUpper.y = playerPos->y;
        } else if (playerPos->y < rangeLower.y) {
            rangeLower.y = playerPos->y;
        }
        if (playerPos->z > rangeUpper.z) {
            rangeUpper.z = playerPos->z;
        } else if (playerPos->z < rangeLower.z) {
            rangeLower.z = playerPos->z;
        }
    }

    //Size of each range (if there's only one player, this will be the same as their position)
    EXVector3 rangeSizes = {
        .x = rangeUpper.x - rangeLower.x,
        .y = rangeUpper.y - rangeLower.y,
        .z = rangeUpper.z - rangeLower.z
    };

    //Middle point of the ranges
    //middle = lower range + (range size / 2)
    middle->x = rangeLower.x + (rangeSizes.x / 2.0);
    middle->y = rangeLower.y + (rangeSizes.y / 2.0);
    middle->z = rangeLower.z + (rangeSizes.z / 2.0);

    //Biggest range of the 3 axis
    *biggestRange = rangeSizes.x;
    if (rangeSizes.y > *biggestRange) {
        *biggestRange = rangeSizes.y;
    }
    if (rangeSizes.z > *biggestRange) {
        *biggestRange = rangeSizes.z;
    }

    return true;
}

//Code that runs prior to a player's SEUpdate.
//Apply hotswap values if players have joined.
void PlayerHandlerPreUpdate(int* self) {
    gpPlayer = self;
    gpPlayerItem = (int*) *self;

    int ID = *(self + 0x8/4);

    for (int i = 0; i < 4; i++) {
        if (players[i] == ID) {
            g_PadNum = i;
            globalRefPortNr = i;
            lastPlayerUpdated = i;

            if (!OnlyPlayer1Exists()) {
                //Set the global breath to this player's breath
                gPlayerState.CurrentBreath = PLAYER_BREATHS[i];
                gPlayerState.Health = PLAYER_HEALTH[i];

                if (PLAYER_INVINCIBILITY[i]) {
                    gPlayerState.AbilityFlags |= Abi_Invincibility;
                } else {
                    gPlayerState.AbilityFlags &= ~Abi_Invincibility;
                }
                gPlayerState.InvincibleTimer = PLAYER_INVINCIBILITY_TIMER[i];

                if (PLAYER_SUPERCHARGE[i]) {
                    gPlayerState.AbilityFlags |= Abi_SuperCharge;
                } else {
                    gPlayerState.AbilityFlags &= ~Abi_SuperCharge;
                }
                gPlayerState.SuperchargeTimer = PLAYER_SUPERCHARGE_TIMER[i];

                EXVector_Copy(&ray0, &PLAYER_RAYVECS[i].vecs[0]);
                EXVector_Copy(&ray1, &PLAYER_RAYVECS[i].vecs[1]);
                EXVector_Copy(&ray2, &PLAYER_RAYVECS[i].vecs[2]);
            }
        }
    }
}

//Code that runs after a player's SEUpdate.
//Store resulting hotswap values for the player.
void PlayerHandlerPostUpdate(int* self) {
    //After the update, we store the playerstate globals that resulted from the update.
    if (OnlyPlayer1Exists()) { return; }

    int ID = *(self + 0x8/4);
    for (int i = 0; i < 4; i++) {
        if (players[i] == ID) {
            lastPlayerUpdated = i;

            PLAYER_BREATHS[i] = gPlayerState.CurrentBreath;
            PLAYER_HEALTH[i] = gPlayerState.Health;

            PLAYER_INVINCIBILITY_TIMER[i] = gPlayerState.InvincibleTimer;
            if ((gPlayerState.AbilityFlags & Abi_Invincibility) != 0) {
                PLAYER_INVINCIBILITY[i] = true;
            } else {
                PLAYER_INVINCIBILITY[i] = false;
            }

            PLAYER_SUPERCHARGE_TIMER[i] = gPlayerState.SuperchargeTimer;
            if ((gPlayerState.AbilityFlags & Abi_SuperCharge) != 0) {
                PLAYER_SUPERCHARGE[i] = true;
            } else {
                PLAYER_SUPERCHARGE[i] = false;
            }

            EXVector_Copy(&PLAYER_RAYVECS[i].vecs[0], &ray0);
            EXVector_Copy(&PLAYER_RAYVECS[i].vecs[1], &ray1);
            EXVector_Copy(&PLAYER_RAYVECS[i].vecs[2], &ray2);

            break;
        }
    }

    //check if vtable is ball gadget
    if ((*(self + 0x4/4) == BALLGADGET_VTABLE)) {
        int* playerStateFlags = self + (0x580/4);

        //Non-final player is dead and death mode is PlayerDespawn
        if (((*playerStateFlags & 2) != 0) && !handlerIsOnlyPlayerLeft(self) && (deathMode == PlayerDespawn)) {
            int portNr = GetPortNrFromPlayerHandler(self);

            gPlayerState.Health = 0xA0;
            PLAYER_HEALTH[portNr] = 0xA0;

            playerJoinCooldownTimers[portNr] = playerJoinCooldownTimerMax;
            showCoolDownTimer = true;
            
            removePlayer(portNr, true);
            //ig_printf("deleting ballgadget\n");
        }
    }
}

//Code that runs before Sparx updates.
//Follow the first available player.
//Make Sparx react to the lowest health among players.
void SparxPreUpdate(int* self) {
    if (players[0] == -1) { return; }

    int* list[4];
    int count = GetArrayOfPlayerHandlers(&list);
    if (count != 0) {
        int* playerHandler = list[0];
        gpPlayer = playerHandler;
        gpPlayerItem = (int*) *playerHandler;
        globalRefPortNr = GetPortNrFromPlayerHandler(playerHandler);
    }

    //If using multiplayer health mode, set to lowest of all player's health.
    //This is partially to make him chase butterflies even if one player isn't at full health
    if (NumberOfPlayers() > 1) {
        gPlayerState.Health = PLAYER_HEALTH[0];
        for (int i = 1; i < 4; i++) {
            if (players[i] != -1) {
                if (gPlayerState.Health > PLAYER_HEALTH[i]) {
                    gPlayerState.Health = PLAYER_HEALTH[i];
                }
            }
        }
    }
}

//Code that runs before any other handler updates.
//Set global references to the closest player.
void MiscHandlerPreUpdate(int* self) {
    int* list[4];
    int count = GetArrayOfPlayerHandlers(&list);
    int vtable = *(self + 0x4/4);
    
    if (count != 0) {
        //Keep track of the closest item so far
        int* closest = NULL;
        float closestDist;

        //get own position
        int* item = (int*) *self;
        EXVector* pos = (EXVector*) (item + (0xD0/4));

        for (int i = 0; i < count; i++) {
            int* handler = list[i];

            int* playerItem = (int*)(*(handler));
            EXVector* playerPos = (EXVector*) (playerItem + (0xD0/4));

            float dist = EXVector_Dist(pos, playerPos);

            if (i == 0) {
                //First player we've found
                closestDist = dist;
                closest = handler;
                continue;
            }

            if (dist < closestDist) {
                //We found a new closest item
                closestDist = dist;
                closest = handler;
            }
        }

        if (closest != NULL) {
            g_PadNum = GetPortNrFromPlayerHandler(closest);
            gpPlayer = closest;
            gpPlayerItem = (int*) *closest;
            globalRefPortNr = g_PadNum;
        }
    }
}

void CameraBallGadgetFollowPostUpdate(int* self) {
    EXVector3 middle = {0};
    float biggestRange = 0;

    if (!GetPlayerPosMidAndRanges(&middle, &biggestRange)) {
        return;
    }

    float* camElevation = (float*) (self + (0x248/4));
    float* camRange = (float*) (self + (0x24C/4));

    //Make the camera zoom out the further apart the players get
    float buffer = 10.0;
    float factor = 1.1;
    if (biggestRange > buffer) {
        //Amount of additional distance compared to default
        float distSize = (biggestRange-buffer);

        //*camRange = (distSize * factor) + buffer;
        Camera_BallGadgetFollow_Distance_UpperLimit = (distSize * factor) + buffer;
        *camElevation = 1.19380522 - (distSize * 0.003);
    } else {
        Camera_BallGadgetFollow_Distance_UpperLimit = 10.0;
        *camElevation = 1.19380522;
    }

    updateCameraTargetItem();
    if (cameraTargetItem != NULL) {
        int** targetItem = (int**) (self + (0x398/4));
        *targetItem = cameraTargetItem;

        EXVector* targetPos = (EXVector*) (cameraTargetItem + (0xD0/4));

        //Set the target item's position to our middle vector
        targetPos->x = middle.x;
        targetPos->y = middle.y;
        targetPos->z = middle.z;
    }

    //if (isButtonPressed(Button_Z, 0)) {
    //    ig_printf("%.2f, %.2f\n", *camElevation, *camRange);
    //}
}

//Code that runs after the follower camera's SEUpdate.
//Manipulate camera variables to make them work with multiple players.
void CameraFollowPostUpdate(int* self) {
    EXVector3 middle = {0};
    float biggestRange = 0;

    if (!GetPlayerPosMidAndRanges(&middle, &biggestRange)) {
        return;
    }

    float* camElevation = (float*) (self + (0x248/4));
    float* camRange = (float*) (self + (0x24C/4));

    //Make the camera zoom out the further apart the players get
    float buffer = 7.0;
    float factor = 1.1;
    if (biggestRange > buffer) {
        //Amount of additional distance compared to default
        float distSize = (biggestRange-buffer);

        *camRange = (distSize * factor) + buffer;
        *camElevation = Camera_Follow_Elevation_Default - (distSize * 0.003);
    } else {
        *camElevation = Camera_Follow_Elevation_Default;
    }

    //Set the camera's target item to our custom one
    updateCameraTargetItem();
    if (cameraTargetItem != NULL) {
        int** targetItem = (int**) (self + (0x398/4));
        *targetItem = cameraTargetItem;

        EXVector* targetPos = (EXVector*) (cameraTargetItem + (0xD0/4));

        //Set the target item's position to our middle vector
        targetPos->x = middle.x;
        targetPos->y = middle.y;
        targetPos->z = middle.z;
    }
}

void CameraBossPostUpdate(int* self) {
    EXVector3 middle = {0};
    float biggestRange = 0;

    if (!GetPlayerPosMidAndRanges(&middle, &biggestRange)) {
        return;
    }

    float* camRange = (float*) (self + (0x410/4));

    //Make the camera zoom out the further apart the players get

    float buffer = 8.0;
    //Special case for final boss since the camera is further zoomed out
    if ((int)GetSpyroMap(0) == gMap_MechaRed) {
        buffer = 12.0;
    }
    float factor = 1.2;
    if (biggestRange > buffer) {
        //Amount of additional distance compared to default
        float distSize = (biggestRange-buffer);

        *camRange = (distSize * factor) + buffer;
    }

    //Set the camera's target item to our custom one
    updateCameraTargetItem();
    if (cameraTargetItem != NULL) {
        //int** targetItem = (int**) (self + (0x398/4));
        //*targetItem = cameraTargetItem;

        EXVector* targetPos = (EXVector*) (cameraTargetItem + (0xD0/4));

        //Set the target item's position to our middle vector
        targetPos->x = middle.x;
        targetPos->y = middle.y;
        targetPos->z = middle.z;
    }
}

//Draw the player's name, health and powerup status
void DrawPlayerMarker(int portNr) {
    int* handler = ItemEnv_FindUniqueIDHandler(&theItemEnv, players[portNr], 0);
    if (handler == NULL) { return; }
    
    //Get item and position
    int* item = (int*) *handler;
    EXVector* playerPos = (EXVector*) (item + (0xD0/4));

    //Display info 2 units above character pos
    EXVector pos = {
        .x = playerPos->x,
        .y = playerPos->y + 2.0,
        .z = playerPos->z,
        .w = playerPos->w
    };

    //Get the screen position for the position
    EXVector2 screenPos = {0.0};
    WorldToDisp(&screenPos, &pos);

    //To make sure it doesn't clip the edges
    static float xBuffer = 20.0;
    static float yBuffer = 40.0;

    //Now we decide the actual position of the marker by making it stick to the edges if offscreen

    //Clamp vertical
    if (screenPos.y < yBuffer) {
        screenPos.y = yBuffer;
    }
    if (screenPos.y > FRAME_SIZE_Y) {
        screenPos.y = FRAME_SIZE_Y;
    }
    //Clamp horizontal
    if (screenPos.x < xBuffer) {
        screenPos.x = xBuffer;
    }
    if (screenPos.x > FRAME_SIZE_X-xBuffer) {
        screenPos.x = FRAME_SIZE_X-xBuffer;
    }

    //If it's not in front of the camera, make it strictly stick to the edges
    //and invert their positions to avoid weirdness
    if (!isInFrontOfCam(&pos)) {
        //Height may vary, but x position must strictly be either all the way left/right
        if (screenPos.x < (FRAME_SIZE_X/2)) {
            screenPos.x = xBuffer;
        } else {
            screenPos.x = FRAME_SIZE_X-xBuffer;
        }

        //Invert X and Y position
        screenPos.y = FRAME_SIZE_Y - screenPos.y + yBuffer;
        screenPos.x = FRAME_SIZE_X - screenPos.x;
    }

    //Position to display name
    EXVector2 textPos = {
        .x = screenPos.x - 10.0,
        .y = screenPos.y - 30.0
    };

    //Name

    textPrint(
        PLAYER_NAMES[portNr], 0,
        (int)textPos.x,
        (int)textPos.y,
        TopLeft, PLAYER_COLORS[portNr], 1.0f
    );

    //Health indicator

    EXRect r = {
        (int) screenPos.x -3 - 18,
        (int) screenPos.y -3 - 20,
        6,
        6
    };

    XRGBA* col = GetHealthColor(PLAYER_HEALTH[portNr]);
    
    //Logic to make the indicator flash red if player is on 1HP
    static int flashingTimer = 0;
    flashingTimer++;
    if ((flashingTimer > 20) && (col == &COLOR_BLACK)) {
        col = &COLOR_RED;
    }
    if (flashingTimer > 40) { flashingTimer = 0; }

    Util_DrawRect(gpPanelWnd, &r, col);

    //Powerup status

    XRGBA* powerUpCol = NULL;
    float current;
    float max;

    //Decide the size and color of the bar
    if (PLAYER_INVINCIBILITY[portNr]) {
        powerUpCol = &COLOR_INV;
        current = PLAYER_INVINCIBILITY_TIMER[portNr];
        max = gPlayerState.InvincibleTimerMax;
    } else if (PLAYER_SUPERCHARGE[portNr]) {
        powerUpCol = &COLOR_SUP;
        current = PLAYER_SUPERCHARGE_TIMER[portNr];
        max = gPlayerState.SuperchargeTimerMax;
    }

    //if no color was decided, don't draw it.
    if (powerUpCol != NULL) {
        static size = 30;

        EXRect r = {
            (int) screenPos.x - 18,
            (int) screenPos.y - 37,
            size,
            6
        };

        Util_DrawRect(gpPanelWnd, &r, &COLOR_BLACK);

        r.w *= (current/max);

        Util_DrawRect(gpPanelWnd, &r, powerUpCol);
    }
}

void DrawMultiplayerMenu() {
    static int xOff = 260;
    static int yOff = 120;
    int textOff = 0;
    
    EXRect rect = {
        .x = xOff - 5,
        .y = yOff - 5,
        .w = FRAME_SIZE_X - xOff,
        .h = 200
    };

    XRGBA col = {
        .r = 0,
        .g = 0,
        .b = 0,
        .a = 0x40 
    };

    Util_DrawRect(gpPanelWnd, &rect, &col);

    textPrint("Multiplayer Options", 0, xOff, yOff, TopLeft, &COLOR_WHITE, 1.3);
    textOff += 30;

    char* deathModeStr;

    switch (deathMode) {
        case ReloadGame:
            deathModeStr = "Reload Game";
            break;
        case PlayerDespawn:
            deathModeStr = "Despawn Player";
            break;
        default:
            deathModeStr = "N/A";
            break;
    }

    textPrint("(Dpad Left/Right: Change)", 0, xOff, yOff + textOff, TopLeft, &COLOR_LIGHT_BLUE, 1.0);
    textOff += 20;
    textPrintF(xOff, yOff + textOff, TopLeft, &COLOR_WHITE, 1.0, "Death Mode: %s", deathModeStr);
    textOff += 40;

    if (deathMode == PlayerDespawn) {
        textPrint("(L/R: Change)\n(X: Faster)\n(Y: Default)", 0, xOff, yOff + textOff, TopLeft, &COLOR_LIGHT_BLUE, 1.0);
        textOff += 60;
        textPrintF(xOff, yOff + textOff, TopLeft, &COLOR_WHITE, 1.0, "Player Respawn Cooldown:\n%.1f Seconds", (float)playerJoinCooldownTimerMax/60.0);
    }
}

void HandleMultiplayerMenu() {
    if (isButtonPressed(Button_Dpad_Left, 0)) {
        deathMode--;
        if ((int)deathMode < 0) {
            deathMode = DEATHMODE_AMOUNT-1;
        }
        PlaySFX(HT_Sound_SFX_GEN_HUD_SELECT);
    } else if (isButtonPressed(Button_Dpad_Right, 0)) {
        deathMode++;
        if (deathMode >= DEATHMODE_AMOUNT) {
            deathMode = 0;
        }
        PlaySFX(HT_Sound_SFX_GEN_HUD_SELECT);
    }

    if (deathMode == PlayerDespawn) {
        int speed = isButtonDown(Button_X, 0) ? 10 : 3;

        if (isButtonDown(Button_R, 0)) {
            playerJoinCooldownTimerMax += speed;
            if (playerJoinCooldownTimerMax > 60*9999) { //Upper limit: 9999 seconds
                playerJoinCooldownTimerMax = 60*9999;
            }
        } else if (isButtonDown(Button_L, 0)) {
            playerJoinCooldownTimerMax -= speed;
            if (playerJoinCooldownTimerMax < 0) { //Lower limit: 0 seconds
                playerJoinCooldownTimerMax = 0;
            }
        } else {
            //Round to nearest 10th of a second when no button is pressed
            playerJoinCooldownTimerMax -= playerJoinCooldownTimerMax % 6;
        }

        if (isButtonPressed(Button_Y, 0)) {
            playerJoinCooldownTimerMax = PLAYER_JOIN_COOLDOWN_MAX_DEFAULT;
        }
    }

    DrawMultiplayerMenu();
}

//main_hook.s | Runs every frame
void MainUpdate() {
    //Setup vtable hooks
    vtable_GUI_PanelItem_v_StateRunning = GUI_PanelItem_v_StateRunning_hookFunc;
    vtable_GUI_PauseMenu_v_DrawStateRunning = GUI_PauseMenu_v_DrawStateRunning_hookFunc;
    vtable_LoadingLoopDraw = LoadingLoopDraw_hookFunc;

    //If the gameloop isn't running, abort
    if ((SE_GameLoop_State != 3) || GameIsPaused()) {
        for (int i = 0; i < 4; i++) {
            playerLeaveTimers[i] = 0;
            playerRestoreTimers[i] = 0;
        }
        return;
    }

    //debug
    if (isButtonDown(Button_Dpad_Down, 0)) {
        showDebugTimer++;
        if (showDebugTimer == 60) {
            showDebug = !showDebug;
        }
    } else {
        showDebugTimer = 0;
    }

    updatePlayerList();

    //If there are no players, we're probably not in a state where we want new ones to join.
    //We'll also wanna reset the join timers
    if (NumberOfPlayers() == 0) {
        playerJoinCooldownTimers[0] = 0;
        playerJoinCooldownTimers[1] = 0;
        playerJoinCooldownTimers[2] = 0;
        playerJoinCooldownTimers[3] = 0;
        return;
    }

    //Update cooldown timers
    for (int i = 0; i < 4; i++) {
        playerJoinCooldownTimers[i]--;
        if (playerJoinCooldownTimers[i] < 0) {
            playerJoinCooldownTimers[i] = 0;
        }
    }

    //If player 1 is holding down dpad left, reset health and restart game.
    if (isButtonDown(Button_Dpad_Left, 0)) {
        restartGameTimer++;
        if (restartGameTimer >= restartGameTimerMax) {
            SetAllHealthFull();
            PlayerState_RestartGame(&gPlayerState);
        }
        if (restartGameTimer > restartGameTimerMax) {
            restartGameTimer = restartGameTimerMax;
        }
    } else {
        restartGameTimer = 0;
    }

    //PRE: There is at least one player alive, and we're ready for them to leave or others to join

    if (OnlyPlayer1Exists()) {
        PLAYER_HEALTH[0]              = gPlayerState.Health;
        PLAYER_BREATHS[0]             = gPlayerState.CurrentBreath;
        PLAYER_INVINCIBILITY[0]       = (gPlayerState.AbilityFlags & Abi_Invincibility) != 0;
        PLAYER_INVINCIBILITY_TIMER[0] = gPlayerState.InvincibleTimer;
        PLAYER_SUPERCHARGE[0]         = (gPlayerState.AbilityFlags & Abi_SuperCharge) != 0;
        PLAYER_SUPERCHARGE_TIMER[0]   = gPlayerState.SuperchargeTimer;
    }

    for (int i = 0; i < 4; i++) {
        if (players[i] == -1) { //Player slot not taken
            if (isButtonPressed(Button_A, i) && (playerJoinCooldownTimers[i] == 0)) {
                addNewPlayer(i);
            }
            //Reset timers if player is missing
            playerLeaveTimers[i] = 0;
            playerRestoreTimers[i] = 0;
        } else { //Player slot taken
            int* handler = ItemEnv_FindUniqueIDHandler(&theItemEnv, players[i], 0);
            bool dying = false;
            if (handler != NULL) {
                PlayerModes mode = (PlayerModes) *(handler + (0x834/4));
                dying = modeIsDying(mode);
            }

            if (isButtonDown(Button_Dpad_Right, i) && !dying) {
                playerLeaveTimers[i]++;
                if (playerLeaveTimers[i] >= playerLeaveTimerMax) {
                    playerLeaveTimers[i] = 0;
                    removePlayer(i, false);
                }
            } else {
                playerLeaveTimers[i] = 0;
            }

            if (isButtonDown(Button_Dpad_Up, i) && !dying) {
                playerRestoreTimers[i]++;
                if (playerRestoreTimers[i] >= playerRestoreTimerMax) {
                    playerRestoreTimers[i] = 0;
                    restorePlayerControl(i);
                }
            } else {
                playerRestoreTimers[i] = 0;
            }
        }
    }

    return;
}

//draw_hook.s | Runs every frame during the HUD draw loop
//Drawing stuff to the screen should be done here to avoid garbled textures
void DrawUpdate() {
    //Draw markers for the players if player 1 isn't alone.
    if ((players[0] == -1) || (NumberOfPlayers() > 1)) {
        for (int i = 0; i < 4; i++) {
            if (players[i] != -1) {
                DrawPlayerMarker(i);
            }
        }
    }

    //Show notif saying Sparx can't be played in multiplayer
    if (NumberOfPlayers() == 0) {
        bool showNoSparxNotif = false;
        static int noSparxNotifTimer = 0;

        if (gpPlayer != NULL) {
            int vtable = *(gpPlayer + 0x4/4);
            if (vtable == SPARX_PLAYER_VTABLE) {
                for (int i = 1; i < 4; i++) {
                    if (isButtonPressed(Button_A, i)) {
                        showNoSparxNotif = true;
                        break;
                    }
                }
            }
        }

        if (showNoSparxNotif) {
            noSparxNotifTimer = 60;
        }
        
        if (noSparxNotifTimer > 0) {
            textPrint("Sparx not available for multiplayer", 0, 0, 0, BottomCentre, &COLOR_LIGHT_RED, 1.0);
            noSparxNotifTimer--;
        }
    }

    //Modes_Sparx sparxMode = *(gpSparx + (0x4d0/4));
    //textPrintF(0, 0, Centre, &COLOR_WHITE, 1.0f, "%d", sparxMode);

    //Draw player notifications
    for (int i = 0; i < 4; i++) {
        bool alreadyShowing = false;

        //LEAVING NOTIFICATION
        if (playerLeaveTimers[i] > 20) {
            if (!alreadyShowing) {
                textPrintF(10, playerNotifYOffets[i], TopLeft, PLAYER_COLORS[i], 1.0f, "P%d: Leaving...", i+1);

                EXRect r = {
                    .x = 140,
                    .y = playerNotifYOffets[i] + 8,
                    .w = 50,
                    .h = 10
                };

                Util_DrawRect(gpPanelWnd, &r, &COLOR_BLACK);

                //Subtract 20 to make the timer start from the left
                r.w = (float)r.w * ((float)(playerLeaveTimers[i]-20) / (float)(playerLeaveTimerMax-20));

                Util_DrawRect(gpPanelWnd, &r, &COLOR_RED);

                alreadyShowing = true;
            }
        }

        //LEAVE NOTIFICATION
        if (playerLeaveNotifTimers[i] > 0) {
            if (!alreadyShowing) {
                char* cause = leaveBecauseDeath[i] ? "died" : "left";

                textPrintF(10, playerNotifYOffets[i], TopLeft, PLAYER_COLORS[i], 1.0f, "P%d: %s", i+1, cause);
                alreadyShowing = true;
            }
            
            playerLeaveNotifTimers[i]--;
        }

        //MAKING UNSTUCK NOTIFICATION
        if (playerRestoreTimers[i] > 20) {
            if (!alreadyShowing) {
                textPrintF(10, playerNotifYOffets[i], TopLeft, PLAYER_COLORS[i], 1.0f, "P%d: Restoring...", i+1);

                EXRect r = {
                    .x = 140,
                    .y = playerNotifYOffets[i] + 8,
                    .w = 50,
                    .h = 10
                };

                Util_DrawRect(gpPanelWnd, &r, &COLOR_BLACK);

                //Subtract 20 to make the timer start from the left
                r.w = (float)r.w * ((float)(playerRestoreTimers[i]-20) / (float)(playerRestoreTimerMax-20));

                Util_DrawRect(gpPanelWnd, &r, &COLOR_GREEN);

                alreadyShowing = true;
            }
        }

        //NOT LOADED YET NOTIFICATION
        if (notLoadedYetNotifTimers[i] > 0) {
            if (!alreadyShowing) {
                textPrintF(10, playerNotifYOffets[i], TopLeft, PLAYER_COLORS[i], 1.0f, "P%d: Not loaded yet, please wait...", i+1);
                alreadyShowing = true;
            }
            
            notLoadedYetNotifTimers[i]--;
        }

        //CHANGED BREATH NOTIFICATION
        if (breathSelectNotifTimers[i] > 0) {
            if (!alreadyShowing) {
                char* breathName = GetBreathName(PLAYER_BREATHS[i]);
                textPrintF(10, playerNotifYOffets[i], TopLeft, PLAYER_COLORS[i], 1.0f, "P%d: Selected %s", i+1, breathName);
                alreadyShowing = true;
            }
            
            breathSelectNotifTimers[i]--;
        }
        
        //JOIN NOTIFICATION
        if (playerJoinedNotifTimers[i] > 0) {
            if (!alreadyShowing) {
                textPrintF(10, playerNotifYOffets[i], TopLeft, PLAYER_COLORS[i], 1.0f, "P%d: Joined!", i+1);
                alreadyShowing = true;
            }
            playerJoinedNotifTimers[i]--;
        }

        if (!alreadyShowing && (playerJoinCooldownTimers[i] > 0)) {
            float time = (float)playerJoinCooldownTimers[i] / 60;

            textPrintF(10, playerNotifYOffets[i], TopLeft, &COLOR_WHITE, 1.0f, "P%d: %.1f", i+1, time);
        }

        playerNotifShowing[i] = alreadyShowing;
    }

    //Draw restart game timer
    if (restartGameTimer > 20) {
        textPrint("Restarting...", 0, 10, 330, TopLeft, &COLOR_LIGHT_RED, 1.2);

        EXRect r = {
            .x = 10,
            .y = 360,
            .w = 120,
            .h = 20
        };

        Util_DrawRect(gpPanelWnd, &r, &COLOR_BLACK);

        r.w = (float)r.w * ((float)(restartGameTimer-20) / (float)(restartGameTimerMax-20));

        Util_DrawRect(gpPanelWnd, &r, &COLOR_RED);
    }

    if (doMultiplayerOptions) {
        doMultiplayerOptions = false;

        HandleMultiplayerMenu();
    }

    //DEBUG BELOW:
    
    if (!showDebug) { return; }

    textSmpPrintF(20, 100, "tgt: %x", cameraTargetItem);

    for (int i = 0; i < 4; i++) {
        int* handler = ItemEnv_FindUniqueIDHandler(&theItemEnv, players[i], 0);
        textSmpPrintF(20, 120 + (i * 30), "%d: %x | %x", i+1, players[i], handler);

        if (handler != NULL) {
            PlayerModes mode = (PlayerModes) *(handler + (0x834/4));
            textSmpPrintF(20, 120 + (i * 30) + 15, "mode: %d", mode);
        }
    }

    if (cameraTargetItem != NULL) {
        EXVector* pos = (EXVector*) (cameraTargetItem + (0xD0/4));

        drawSquareAtVec(pos, 4, &COLOR_RED);
    }

    textSmpPrintF(20, 240, "items: %d", ItemEnv_ItemCount);

    if (gpGameWnd != NULL) {
        textSmpPrintF(20, 255, "cam: %x", (int*) *(gpGameWnd + (0x378/4)));
    }

    textSmpPrintF(20, 270, "map: %x", GetSpyroMap(0));

    return;
}

int GUI_PauseMenu_v_DrawStateRunning_Hook(int* self, int* pWnd) {
    float* xPosition = (float*) (self + (0xCC/4));
    *xPosition-= 15.0;
    if (*xPosition < 370.0) {
        *xPosition = 370.0;
        doMultiplayerOptions = true;
    }

    return GUI_PauseMenu_v_DrawStateRunning(self, pWnd);
}

//scan_hook.s | On the frame this function returns true, scanmode will be enabled/disabled.
//Typically you'd return true if a button has just been pressed.
bool ScanUpdate() {
    //return isButtonPressed(Button_Z, g_PadNum);
    return false;
}

//runs right before an itemhandler is updated
bool ItemHandler_SEUpdate_Hook(int* self) {
    int vtable = *(self + 0x4/4);
    int ID = *(self + 0x8/4);

    if (HandlerIsPlayer(self)) {
        //This function will set the playerstate globals to the player's personal one
        PlayerHandlerPreUpdate(self);

        bool res = ItemHandler_SEUpdate(self);

        PlayerHandlerPostUpdate(self);

        g_PadNum = 0;
        return res;
    } else if (vtable == CAMERA_FOLLOW_VTABLE) {
        //Make the player moving the stick the most control the camera
        g_PadNum = whoShouldControlCamera();

        bool res = ItemHandler_SEUpdate(self);

        CameraFollowPostUpdate(self);

        g_PadNum = 0;
        return res;
    } else if (vtable == CAMERA_BALLGADGET_FOLLOW_VTABLE) {
        //Make the player moving the stick the most control the camera
        g_PadNum = whoShouldControlCamera();

        bool res = ItemHandler_SEUpdate(self);

        CameraBallGadgetFollowPostUpdate(self);

        g_PadNum = 0;
        return res;
    } else if (vtable == CAMERA_BOSS_VTABLE) {
        g_PadNum = whoShouldControlCamera();

        updateCameraTargetItem();
        if (cameraTargetItem != NULL) {
            gpPlayerItem = cameraTargetItem;
        }

        bool res = ItemHandler_SEUpdate(self);

        CameraBossPostUpdate(self);

        int* list[4];
        int count = GetArrayOfPlayerHandlers(&list);
        if (count != 0) {
            gpPlayerItem = (int*) *(list[0]);
        } else {
            gpPlayerItem = NULL;
        }

        g_PadNum = 0;
        return res;
    } else if (vtable == SPARX_VTABLE) {
        SparxPreUpdate(self);
    } else if (vtable == LOCKEDCHEST_VTABLE) {
        for (int i = 0; i < 4; i++) {
            if (players[i] == -1) { continue; }

            SetPlayerRefToPort(i);
            break;
        }
    } else if (vtable == BOSS_VTABLE) {
        //gpPlayer = GetFirstPlayerNotZeroHP();
        //gpPlayerItem = (int*) *gpPlayer;
    } else {
        MiscHandlerPreUpdate(self);
    }

    //This is only reached if each case doesn't return on its own
    bool res = ItemHandler_SEUpdate(self);

    //Return control to player 1 after updating
    g_PadNum = 0;
    return res;
}

//Replaces the call to EXItemEnv::UpdateItems_Physics
//replaces the original code but adds swapping around player references.
void EXItemEnv_UpdateItems_Physics_Hook(int* self) {
    EXDList* list = (EXDList*) *(self + (0x80/4));

    EXDListItem* i = list->head;

    while (i != NULL) {
        int* vtable = (int*) *(((int*)i) + (0x18/4));

        int* handler = *(((int*)i) + (0x14C/4));

        bool isPlayer = HandlerIsPlayer(handler);
        if (isPlayer) {
            gpPlayer = handler;
            gpPlayerItem = i;
            globalRefPortNr = GetPortNrFromPlayerHandler(handler);
        }

        EXItem_DoItemPhysics doPhysFunc = (EXItem_DoItemPhysics) *(vtable + (0x28/4));
        doPhysFunc(i);

        i = i->next;
    }
}

//Runs before the game checks for the player changing breath
//Only used for the "player changed breath" notification on the HUD
bool TestBreathChangeHook(int* self) {
    //Check if the breath changed before running any logic
    bool changed = Spyro_TestBreathChange(self);

    if (changed) {
        //Search for the player who changed breath
        int player = -1;
        int ID = *(self + 0x8/4);

        for (int i = 0; i < 4; i++) {
            if (players[i] == ID) {
                player = i;
                break;
            }
        }

        if (player != -1) {
            //Set notification settings
            breathSelectNotifTimers[player] = 60;
            lastBreathChangedTo = gPlayerState.CurrentBreath;
        }
    }

    return changed;
}

//After sparx eats a butterfly (add 1 unit of health to all players)
void Sparx_SetPlayerHealth_Hook(int* self, int health) {
    //Check if we should use the multiplayer health mode
    if (NumberOfPlayers() > 1) {
        for (int i = 0; i < 4; i++) {
            if (players[i] != -1) {
                //Get the player's stored health
                gPlayerState.Health = PLAYER_HEALTH[i];
                //Add 1 unit of health
                PlayerState_SetHealth(self, gPlayerState.Health + 0x20);
                //Save the resulting health value to the player
                PLAYER_HEALTH[i] = gPlayerState.Health;
            }
        }
    } else {
        PlayerState_SetHealth(self, health);
    }
}

//After a special butterfly is collected (set all health to full)
void Butterfly_Special_SetHealth_Hook(int* self, int health) {
    gPlayerState.Health = 0xA0;
    PLAYER_HEALTH[0] = 0xA0;
    PLAYER_HEALTH[1] = 0xA0;
    PLAYER_HEALTH[2] = 0xA0;
    PLAYER_HEALTH[3] = 0xA0;
}

//Sets all players' health and the global health to full
void SetAllHealthFull() {
    gPlayerState.Health = 0xA0;
    PLAYER_HEALTH[0] = 0xA0;
    PLAYER_HEALTH[1] = 0xA0;
    PLAYER_HEALTH[2] = 0xA0;
    PLAYER_HEALTH[3] = 0xA0;
}

//Returns whether or not a map inherets from or is an instance of SEMap_MiniGame
bool MapIsMinigame(int* map) {
    if (map == NULL) { return false; }

    int* vtable = (int*) *(map + (0x74/4));
    GetRuntimeClass_func getRTC = (GetRuntimeClass_func) *(vtable + (0x8/4));

    for (EXRuntimeClass* class = getRTC(); class != NULL; class = class->pBaseClass) {
        if (class == 0x803cc500) { //Compare with SEMap_MiniGame::classSEMap_MiniGame
            return true;
        }
    }

    return false;
}

//Replaces function at 0x800a31b0 and 0x80047f4c (both Spyro and Hunter die the same way)
void ReImpl_SpyroHunter_urghhhImDead(int* self) {
    if (handlerIsOnlyPlayerLeft(self) || (deathMode == ReloadGame)) {
        PlayerState_SetHealth(&gPlayerState, 0xA0);
        PlayerState_RestartGame(&gPlayerState);
    } else {
        int portNr = GetPortNrFromPlayerHandler(self);

        gPlayerState.Health = 0xA0;
        PLAYER_HEALTH[portNr] = 0xA0;

        playerJoinCooldownTimers[portNr] = playerJoinCooldownTimerMax;
        showCoolDownTimer = true;
        removePlayer(portNr, true);
    }
}

//Replaces function at 0x80020ebc
void ReImpl_Blink_urghhhImDead(int* self) {
    if (handlerIsOnlyPlayerLeft(self) || (deathMode == ReloadGame)) {
        int* map = PlayerSetupInfo_GetMap(&gPlayerState.Setup);

        if (MapIsMinigame(map)) {
            int* vtable = (int*) *(map + (0x74/4));
            SE_Map_SetMiniGameDie_func setDieFunc = (SE_Map_SetMiniGameDie_func) *(vtable + (0xf8/4));
            setDieFunc(map);

            //Set some flags in the animator
            int* anim = (int*) *(self + (0x144/4));
            ushort* itemFlags = (ushort*) (anim + (0xc/4));
            *itemFlags &= ~((ushort) 1);

            //Set some flags in the player handler
            int* playerStateFlags = self + (0x580/4);
            *playerStateFlags |= 2; //ps_dead
        } else {
            //If it's not a minigame level, just do the usual stuff
            PlayerState_SetHealth(&gPlayerState, 0xA0);
            PlayerState_RestartGame(&gPlayerState);
        }
    } else {
        int portNr = GetPortNrFromPlayerHandler(self);

        gPlayerState.Health = 0xA0;
        PLAYER_HEALTH[portNr] = 0xA0;

        playerJoinCooldownTimers[portNr] = playerJoinCooldownTimerMax;
        showCoolDownTimer = true;
        removePlayer(portNr, true);
    }
}

//Replaces function at 0x8007c440
void ReImpl_SgtByrd_urghhhImDead(int* self) {
    if (handlerIsOnlyPlayerLeft(self) || (deathMode == ReloadGame)) {
        int* map = PlayerSetupInfo_GetMap(&gPlayerState.Setup);

        if (MapIsMinigame(map)) {
            int* vtable = (int*) *(map + (0x74/4));
            SE_Map_SetMiniGameDie_func setDieFunc = (SE_Map_SetMiniGameDie_func) *(vtable + (0xf8/4));
            setDieFunc(map);

            //Set some flags in the player handler
            int* playerStateFlags = self + (0x580/4);
            *playerStateFlags |= 2; //ps_dead
        } else {
            //If it's not a minigame level, just do the usual stuff
            PlayerState_SetHealth(&gPlayerState, 0xA0);
            PlayerState_RestartGame(&gPlayerState);
        }
    } else {
        int portNr = GetPortNrFromPlayerHandler(self);

        gPlayerState.Health = 0xA0;
        PLAYER_HEALTH[portNr] = 0xA0;

        playerJoinCooldownTimers[portNr] = playerJoinCooldownTimerMax;
        showCoolDownTimer = true;
        removePlayer(portNr, true);
    }
}

void GadgetPad_SuperCharge_Hook() {
    if (gpPlayer == NULL) { return; }

    //At this point, the global references should be the player closest to the pad
    int portNr = GetPortNrFromPlayerHandler(gpPlayer);

    PLAYER_SUPERCHARGE[portNr] = (gPlayerState.AbilityFlags & Abi_SuperCharge) != 0;
    PLAYER_SUPERCHARGE_TIMER[portNr] = gPlayerState.SuperchargeTimer; 
}

void GadgetPad_Invincibility_Hook() {
    if (gpPlayer == NULL) { return; }

    //At this point, the global references should be the player closest to the pad
    int portNr = GetPortNrFromPlayerHandler(gpPlayer);

    PLAYER_INVINCIBILITY[portNr] = (gPlayerState.AbilityFlags & Abi_Invincibility) != 0;
    PLAYER_INVINCIBILITY_TIMER[portNr] = gPlayerState.InvincibleTimer;
}

//Edit behavior of icons when running in multiplayer
int GUI_PanelItem_v_StateRunning_Hook(int* self) {
    //Normal behavior if singleplayer
    if (NumberOfPlayers() <= 1) { return GUI_PanelItem_v_StateRunning(self); }
    
    //Store values we don't want displayed on the HUD and set them to zero

    //Store breath
    Breaths tempBreath = gPlayerState.CurrentBreath;
    gPlayerState.CurrentBreath = lastBreathChangedTo;

    //Store invincibility
    PlayerAbilities tempInv = gPlayerState.AbilityFlags & Abi_Invincibility;
    gPlayerState.AbilityFlags &= ~Abi_Invincibility;
    float tempInvTimer = gPlayerState.InvincibleTimer;
    gPlayerState.InvincibleTimer = 0;

    //Store supercharge
    PlayerAbilities tempSup = gPlayerState.AbilityFlags & Abi_SuperCharge;
    gPlayerState.AbilityFlags &= ~Abi_SuperCharge;
    float tempSupTimer = gPlayerState.SuperchargeTimer;
    gPlayerState.SuperchargeTimer = 0;

    //Execute update
    int res = GUI_PanelItem_v_StateRunning(self);

    //Restore the values to their pre-update state

    //Restore breath
    gPlayerState.CurrentBreath = tempBreath;

    //Restore invincibility
    gPlayerState.AbilityFlags |= tempInv;
    gPlayerState.InvincibleTimer = tempInvTimer;

    //Restore supercharge
    gPlayerState.AbilityFlags |= tempSup;
    gPlayerState.SuperchargeTimer = tempSupTimer;

    return res;
}

//Replaces the Delete method for the XSEItemHandler_Player class
int ReImpl_XSEItemHandler_Player_Delete(int* self) {
    Players player = *(self + (0x578/4));

    //If there's more than 1 player, we wanna check if any of the same type is present,
    //so we don't end up unloading files that are used.
    if (NumberOfPlayers() > 1) {
        int portNr = GetPortNrFromPlayerHandler(self);

        for (int i = 0; i < 4; i++) {
            if (i == portNr) { continue; } //ignore self

            //Get the other player's handler
            int* oHandler = ItemEnv_FindUniqueIDHandler(&theItemEnv, players[i], 0);
            if (oHandler == NULL) { continue; }

            //Get other player's player type
            Players oPlayer = *(oHandler + (0x578/4));

            //Now return from the call if we find any other player using the same files.

            //Special case for Spyro, Flame and Ember since they use the same files
            if (
                (player == Player_Spyro) ||
                (player == Player_Ember) ||
                (player == Player_Flame)
            ) {
                if (
                    (oPlayer == Player_Spyro) ||
                    (oPlayer == Player_Ember) ||
                    (oPlayer == Player_Flame)
                ) {
                    return 2;
                }
            }
            
            if (player == oPlayer) {
                return 2;
            }
        }
    }

    PlayerLoader_DeLoad(&gPlayerLoader, player);
    return 2;
}

//Replaces the DoKill method call on the XSEItemHandler class
//when called from one of the players' Delete method when it's trying to delete Sparx
void ReImpl_XSEItemHandler_DoKill(int* self) {
    //We don't wanna unload Sparx if there's more than 1 player
    if (NumberOfPlayersWhoCanHaveSparx() > 1) { return; }

    //This is just the regular code for a single player
    int* trigger = (int*) *(self + (0x10/4));

    if (trigger == NULL) {
        ItemHandler_SEKill(self);
    } else {
        SE_Trigger_DoKill(trigger);
    }

    //We're nop'ing the usual nullification of this, so it must be done here
    gpSparx = NULL;
}

void ChangePlayer_Hook() {
    players[globalRefPortNr] = *(gpPlayer + (0x8/4));
}

//Reimplementation of the loading draw loop.
void LoadingLoopDraw_ReImpl(int* self, int* pWnd) {
    //Set up text drawing for later
    XRGBA textCol = {0, 0x60, 0, 0x80};
    XWnd_SetText(pWnd, HT_File_Panel, HT_Font_Test, &textCol, 1.0f, Centre);

    /*
     * Set up drawing dimensions
     */

    //Create a copy of the display's safeframe rect
    EXRect safeFrame;
    EXRect_Copy(&safeFrame, BaseDisplay_m_pDisplay + (0x34/4));

    //Set the text rect to be slightly smaller than the safeframe
    EXRect* TextRect = (EXRect*)(pWnd + (0x2dc/4));
    TextRect->x = safeFrame.x + 0x20;
    TextRect->y = safeFrame.y + 0x20;
    TextRect->w = safeFrame.w - 0x40;
    TextRect->h = safeFrame.h - 0x40;

    //Get the center of the screen
    EXPoint centerPoint = {
        safeFrame.x + safeFrame.w/2,
        safeFrame.y + safeFrame.h/2
    };

    /*
     * Set up texture to be used for the sectors
     */

    //Load a white 16x16 texture
    int *pTexture = Util_GetGameInfoTexture(HT_Texture_White16x16);

    //(Virtual method call) Select this texture to be used in the current window
    EXWnd_SelectSprite2DTexture(pWnd, pTexture, false, false);

    /*
     * Draw a sprite for some reason.
     * This doesn't even show up, removing has no effect.
     */
    
    //Create sprite object
    XSprite2D sprite2D = {
    /* Col */ (XRGBA){0, 0, 0, 0x80},
    /* x   */ 0,
    /* y   */ 0,
    /* w   */ *(pWnd + (0x8/4)),
    /* h   */ *(pWnd + (0x6/4)),
    /* u1  */ 0.0,
    /* v1  */ 0.0,
    /* u2  */ 1.0,
    /* v2  */ 1.0
    };

    //Draw the sprite on the window
    XSprite2D_Draw(&sprite2D, pWnd);

    /*
     * Set up sector drawing math
     */

    ig_printf("");
    //Reset RNG-seed
    RandSetSeed(&g_EXRandClass, 0);

    //Get update counter (counts up every frame)
    int* updateCounter = (self + (0x6c/4));

    //Make player 2 control this counter
    *updateCounter += (int)(Pads_Analog[1].LStick_Y * 20.0);

    //Scale it down and make it cycle from 0 to 1. Cycles every 1000 frames
    float timeLine = ((float)*updateCounter) * 0.001;
    float timeCycle = ig_fmodf(timeLine, 1.0);

    /*
     * Draw sectors.
     * Hard-coded to draw 30 at all times. 
     */
    for (int i = 0; i < 30; i++) {
        /*
         * Draw two sectors:
         * One with a uniform green colour,
         * one right next to it that fades to black.
         * 
         * Result looks like one whole sector that fades out at its inner edge.
         */

        //temp values
        float n1;
        float n2;

        //Current point in the sector's life cycle. Cycles at the same rate as timeCycle, just offset slightly.
        n1 = Randf(&g_EXRandClass); //Range: 0 to 1
        float lifeCycle = ig_fmodf(timeCycle + n1, 1.0);
        
        //Calculate the radii
        n1 = Randf(&g_EXRandClass) * 4.0 + 10.0; //Range: 10 to 14
        n2 = n1 - (lifeCycle * 10.0);
        n1 = Randf(&g_EXRandClass) * 80.0 + 50.0; //Range: 50 to 130

        float radius2 = 400.0 / n2; //Radius dividing the two sectors
        float radius3 = radius2 + (n1 / n2); //Outer radius
        //Change the outer ring thickness based on player 4 input
        radius3 += (Pads_Analog[3].LStick_Y + 0.5) * 30.0;
        float radius1 = radius2 - (n1 * (1.0/8.0)); //Inner radius

        //Calculate start angle
        n1 = Randf(&g_EXRandClass) * 2*PI; //Range: 0 to 2*PI
        n2 = Randf(&g_EXRandClass); //Range: 0 to 1
        float startAng = timeLine * (n2 * 20.0 - 10.0) + n1;

        //Calculate end angle
        n1 = Randf(&g_EXRandClass) * 80.0 + 16.0; //Range: 16 to 96
        float endAng = n1 * 0.017453292 + startAng;

        //Sector resolution (how many "vertices" there are)
        float sectorRes = (n1 / 12.0) + 1.0;

        /*
         * Calculate colors.
         * Make the sector fade out at the end of its lifecycle.
         */

        float fAlpha = lifeCycle * 1000.0;

        //Color alpha doesn't go above 128
        if (fAlpha > 128.0) {
            fAlpha = 128.0;
        }

        //Start fading sector out at the end of its lifecycle
        if (lifeCycle > 0.85) {
            fAlpha = fAlpha * ((1.0 - lifeCycle) * (2.0/3.0) * 10.0);
        }

        //Clamp between 0 and the max opacity of all sectors
        if (fAlpha < 0.0) {
            fAlpha = 0.0;
        }
        if (fAlpha > 80.0) {
            fAlpha = 80.0;
        }
        
        char cAlpha = (char)fAlpha;

        XRGBA sectorCol = {0, 0x40, 0, cAlpha};

        //Change color based on what player 3 input
        float pad3X = Pads_Analog[2].LStick_X;
        float pad3Y = Pads_Analog[2].LStick_Y;
        if ((pad3X != 0.0) || (pad3Y != 0.0)) {
            sectorCol.g = (int)(pad3X * (float)0x40) + 0x40;

            if (pad3X < 0.0) {
                sectorCol.r += (int)((-pad3X) * (float)0x40);
                sectorCol.b += (int)((-pad3X) * (float)0x40);
            } else {
                sectorCol.r += (int)((pad3X) * (float)0x40);
                sectorCol.b += (int)((pad3X) * (float)0x40);
            }
            
            if (pad3Y > 0.0) {
                sectorCol.r += (int)(pad3Y * (float)0x40);
            } else {
                sectorCol.b += (int)((-pad3Y) * (float)0x40);
            }

            XRGBA_Balance(&sectorCol, 0x20);
        }
        
        XRGBA fadeCol = {0, 0, 0, cAlpha};

        //Set the centerpoint according to player 1 input
        centerPoint.x += (int)(Pads_Analog[0].LStick_X * lifeCycle * 20.0);
        centerPoint.y += (int)(Pads_Analog[0].LStick_Y * lifeCycle * 20.0);

        /*
         * Parameters for DrawSector call are:
         * 1:  Center point
         * 2:  Inner radius
         * 3:  Outer radius
         * 4:  Start angle in radians
         * 5:  End angle in radians
         * 6:  Color of inner vertex 1
         * 7:  Color of outer vertex 1
         * 8:  Color of inner vertex 2
         * 9:  Color of outer vertex 2
         * 10: Resolution of sector (how many "polygons" it is split into)
         * 11: Window where the sector will be drawn
         */

        //Draw outer thick sector
        Util_DrawSector(&centerPoint, radius2, radius3, startAng, endAng, &sectorCol, &sectorCol, &sectorCol, &sectorCol, sectorRes, pWnd);
        //Draw inner slim sector with a gradient
        Util_DrawSector(&centerPoint, radius1, radius2, startAng, endAng, &fadeCol,   &sectorCol, &fadeCol,   &sectorCol, sectorRes, pWnd);
    }

    //Draw "loading" text if this value is 2
    if (*(self + (0x128/4)) == 2) {
        XWnd_TextPrint(pWnd, HT_Text_Loading);
    }

    //Draw all children of this loop in the current window
    SE_Loop_ChildListDraw(self, pWnd);
    return;
}