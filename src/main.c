#include <common.h>
#include <hashcodes.h>
#include <Sound.h>
#include <textprint.h>
#include <buttons.h>
#include <playerstate.h>
#include <symbols.h>
#include <screenmath.h>

struct RayVecs {
    EXVector vecs[3];
};
typedef struct RayVecs RayVecs;

int GUI_PanelItem_v_StateRunning_Hook(int* self);
GUI_PanelItem_v_StateRunning_func GUI_PanelItem_v_StateRunning_hookFunc = GUI_PanelItem_v_StateRunning_Hook;

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

XRGBA* HEALTH_COLORS_NO_UPGRADE[] = {
    /*0x00*/ &COLOR_BLACK,
    /*0x20*/ &COLOR_BLACK,
    /*0x40*/ &COLOR_BLACK,
    /*0x60*/ &COLOR_GREEN,
    /*0x80*/ &COLOR_BLUE,
    /*0xA0*/ &COLOR_P4
};

XRGBA* HEALTH_COLORS_UPGRADE[] = {
    /*0x00*/ &COLOR_BLACK,
    /*0x20*/ &COLOR_BLACK,
    /*0x40*/ &COLOR_RED,
    /*0x60*/ &COLOR_GREEN,
    /*0x80*/ &COLOR_BLUE,
    /*0xA0*/ &COLOR_P4
};

RayVecs PLAYER_RAYVECS[4];

char* PLAYER_NAMES[] = { "P1", "P2", "P3", "P4" };

Breaths PLAYER_BREATHS[] = {
    Breath_Fire,
    Breath_Fire,
    Breath_Fire,
    Breath_Fire
};

int PLAYER_HEALTH[] = { 0xA0, 0xA0, 0xA0, 0xA0 };

bool PLAYER_SUPERCHARGE[] = {false, false, false, false};
float PLAYER_SUPERCHARGE_TIMER[] = { 0.0, 0.0, 0.0, 0.0};

bool PLAYER_INVINCIBILITY[] = {false, false, false, false};
float PLAYER_INVINCIBILITY_TIMER[] = { 0.0, 0.0, 0.0, 0.0};

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
int BLINKYBULLET_VTABLE = 0x80408988;
int LOCKEDCHEST_VTABLE = 0x80429b18;

#define CHARACTER_AMOUNT 8
Players CHARACTERS[] = {
    Player_Spyro,
    Player_Hunter,
    Player_Blinky,
    Player_SgtByrd,
    Player_Sparx,
    Player_BallGadget,
    Player_Ember,
    Player_Flame
};
int CHARACTER_VTABLES[] = {
    0x8040B908, //Spyro
    0x80407338, //Hunter
    0x80406488, //Blink
    0x8040aea8, //Sgt. Byrd
    0x80405e38, //Sparx
    0x80404ec0, //Ball Gadget
    0x804051d8, //Ember
    0x80405428  //Flame
};

bool initialized = false;
bool showDebug = false;
int showDebugTimer = 0;

bool playerNotifShowing[] = {
    false,
    false,
    false,
    false
};
u16 playerNotifYOffets[] = {
    250,
    265,
    280,
    295
};

int playerJoinedNotifTimers[] = {0, 0, 0, 0};

int notLoadedYetNotifTimers[] = {0, 0, 0, 0};

int restartGameTimer = 0;
int restartGameTimerMax = 80;

int breathSelectNotifTimers[] = {0, 0, 0, 0};
Breaths lastBreathChangedTo = Breath_Fire;

int* cameraTargetItem = NULL;

int playerJoinCooldownTimer = 0;

//Contains the itemhandler ID's for each of the four players. -1 if not in use.
int players[4] = {-1, -1, -1, -1};
int lastPlayerUpdated = 0;

//Timers for how long each player has held down the button to restore control/visibility
int player_restore_timers[4] = {0, 0, 0, 0};
int player_restore_timer_max = 60;

int playerRestoreNotifTimers[] = {0, 0, 0, 0};

int player_leave_timers[4] = {0, 0, 0, 0};
int player_leave_timer_max = 90;

int playerLeaveNotifTimers[] = {0, 0, 0, 0};

//Check for the Z button being pressed twice within 20 frames at the given pad number
bool checkZDoublePress(int padNr) {
    static int numPressed[] = {0, 0, 0, 0};
    static int timer[] = {0, 0, 0, 0};

    bool zPressed = isButtonPressed(Button_Z, padNr);

    if (numPressed[padNr] == 0 && !zPressed) {
        //Have not pressed and am not pressing
        timer[padNr] = 0;

        return false;
    }

    //Have either pressed it before or am pressing now
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

//Get the function pointer to the playersetup function for the given map
SE_Map_v_PlayerSetup GetMapPlayerSetupFunc(int* map) {
    int* vtable = (int*) *(map + (0x74/4));

    return *(vtable + (0xc0/4));
}

ItemHandler_Delete GetHandlerDeleteFunc(int* handler) {
    int* vtable = (int*) *(handler + (0x4/4));

    return *(vtable + (0x50/4));
}

uint getPlayerSkinHash(int* handler) {
    if (handler == NULL) { return NULL; }

    int* item = (int*)(*handler);
    int* anim = *(item + (0x144/4));

    if (anim == NULL) { return NULL; }

    int* skinAnim = *(anim + (0x110/4));

    if (skinAnim == NULL) { return NULL; }

    return *(skinAnim + (0xD0/4));
}

//Get the number of player items currently referenced
int NumberOfPlayers() {
    int count = 0;

    for (int i = 0; i < 4; i++) {
        if (players[i] != -1) {
            count++;
        }
    }

    return count;
}

//Get the number of player items currently referenced, who can have Sparx following around
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

bool SetPlayerRefToPort(int portNr) {
    int* handler = ItemEnv_FindUniqueIDHandler(&theItemEnv, players[portNr], 0);
    if (handler == NULL) { return false; }

    g_PadNum = portNr;
    gpPlayer = handler;
    gpPlayerItem = (int*) *handler;

    return true;
}

//Check if the given handler's vtable matches that of a player
bool HandlerIsPlayer(int* handler) {
    int vtable = *(handler + 0x4/4);

    for (int j = 0; j < CHARACTER_AMOUNT; j++) {
        if (CHARACTER_VTABLES[j] == vtable) {
            return true;
        }
    }

    return false;
}

//Returns amount of handlers found, and inserts them into the given list of size 4
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

//Reset all player references and search the item list to populate it again.
void initializePlayers() {
    players[0] = -1;
    players[1] = -1;
    players[2] = -1;
    players[3] = -1;

    if (ItemEnv_ItemCount != 0) {
        EXDListItem* item = ItemEnv_ItemList->head;

        int playerIndex = 0;
        while (item != NULL) {
            int* handler = (int*) *((int*)item + (0x14C/4));
            
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

//Add new player and assign it to the given port number
void addNewPlayer(int portNr) {
    //Only perform on players 2, 3 and 4
    if ((portNr < 1) || (portNr > 3)) { return; }

    //Get current map
    int* map = GetSpyroMap(0);
    if (map == NULL) { return; }

    //Find the handler with the ID in port number 1 (this is where we spawn the new player next to)
    int* handler = ItemEnv_FindUniqueIDHandler(&theItemEnv, players[0], 0);
    if (handler == NULL) { return; }

    //Get item
    int* item = (int*)(*handler);

    //New player is spawned above and to the side of player 1
    EXVector* pos = (EXVector*) (item + (0xD0/4));
    EXVector v = {
        pos->x + 2.0,
        pos->y + 3.0,
        pos->z,
        pos->w
    };

    //Get rotation
    EXVector* rot = (EXVector*) (item + (0xE0/4));

    //Get the player setup function from the map's vtable
    SE_Map_v_PlayerSetup setupFunc = GetMapPlayerSetupFunc(map);

    setupFunc(map, &v, rot);

    //get the "player" field in the global PlayerSetup
    Players* setup_Player = &gPlayerState.Setup.Player;

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
    if (*setup_Player == Player_Spyro) {
        if (portNr == 1) {
            *setup_Player = Player_Ember;
        } else if (portNr == 2) {
            *setup_Player = Player_Flame;
        }
    }

    //Create new player handler
    int* newPlayer = CreatePlayer(map);
    if (newPlayer == NULL) { return; }

    //Grab ID and assign it to the player
    int ID = *(newPlayer + 0x8/4);
    players[portNr] = ID;

    //Finally set some notification values
    playerJoinedNotifTimers[portNr] = 60;
    playerLeaveNotifTimers[portNr] = 0;
}

//Remove player (2, 3 or 4) and set player 1 as the global referent
//THIS CRASHES THE GAME!!! Character file is unloaded, leading to invalid file ref for the other players.
void removePlayer(int portNr) {
    //Only perform on players 2, 3 and 4
    if ((portNr < 1) || (portNr > 3)) { return; }

    if (players[portNr] == -1) { return; }
    
    int* handler = ItemEnv_FindUniqueIDHandler(&theItemEnv, players[portNr], 0);
    if (handler == NULL) { return; }

    //ItemHandler_Delete deleteFunc = GetHandlerDeleteFunc(handler);
    //deleteFunc(handler);
    ItemHandler_SEKill(handler);
    players[portNr] = -1;

    //Get the player 1 handler
    if (players[0] == -1) { return; }
    
    handler = ItemEnv_FindUniqueIDHandler(&theItemEnv, players[0], 0);
    if (handler == NULL) { return; }

    gpPlayer = handler;
    gpPlayerItem = (int*) *handler;

    //Finally set some notification values
    playerJoinedNotifTimers[portNr] = 0;
    playerLeaveNotifTimers[portNr] = 60;
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

    for (int i = 0; i < 4; i++) {
        if (players[i] == -1) { continue; }

        float x = ig_fabsf(Pads_Analog[i].RStick_X);
        float y = ig_fabsf(Pads_Analog[i].RStick_Y);
        float mag = ig_sqrtf((x*x) + (y*y));

        if (i == 0) {
            biggestMag = mag;
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

    return true;
}

//Create camera target item if it doesn't exist or if it's uninitialized
void updateCameraTargetItem() {
    if (!itemExists(cameraTargetItem)) {
        cameraTargetItem = XSEItem_CreateObject();
    }
}

//Make sure the right pad is used for when the player is processed
//Also sets the global references to make things work correctly
void PlayerHandlerPreUpdate(int* self) {
    gpPlayer = self;
    gpPlayerItem = (int*)(*self);

    int ID = *(self + 0x8/4);

    for (int i = 0; i < 4; i++) {
        if (players[i] == ID) {
            g_PadNum = i;

            if (NumberOfPlayers() > 1) {
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

void PlayerHandlerPostUpdate(int* self) {
    //After the update, we store the playerstate globals that resulted from the update.
    if (NumberOfPlayers() <= 1) { return; }

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
}

//Make sure player 1 is referenced when Sparx is updated
void SparxPreUpdate(int* self) {
    if (players[0] == -1) { return; }

    int* handler = ItemEnv_FindUniqueIDHandler(&theItemEnv, players[0], 0);
    if (handler == NULL) { return; }

    gpPlayer = handler;
    gpPlayerItem = (int*) *handler;

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

//Every handler sets the global references to whichever player is closest
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
            gpPlayer = closest;
            gpPlayerItem = (int*)(*closest);
        }
    }
}

//Runs AFTER camera_follow's SEUpdate.
//Manipulate camera variables to make them work with multiple players
void CameraFollowPostUpdate(int* self) {
    int* list[4];
    int count = GetArrayOfPlayerHandlers(&list);

    if (count == 0) { return; }

    //Lower and upper ranges for positions the players take up
    EXVector3 rangeLower = {0.0};
    EXVector3 rangeUpper = {0.0};

    //Loop through all players and gather the ranges of their positions
    for (int i = 0; i < count; i++) {
        int* handler = list[i];

        int* playerItem = (int*)(*(handler));
        EXVector* playerPos = (EXVector*) (playerItem + (0xD0/4));

        //Initialize the range vectors for the first player
        if (i == 0) {
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
    EXVector3 middle = {
        .x = rangeLower.x + (rangeSizes.x / 2.0),
        .y = rangeLower.y + (rangeSizes.y / 2.0),
        .z = rangeLower.z + (rangeSizes.z / 2.0)
    };

    //Biggest range of the 3 axis
    float biggestRange = rangeSizes.x;
    if (rangeSizes.y > biggestRange) {
        biggestRange = rangeSizes.y;
    }
    if (rangeSizes.z > biggestRange) {
        biggestRange = rangeSizes.z;
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

    uint* collideFlags = (uint*) (self + (0x404/4));
    *collideFlags |= 2;
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

    static float xBuffer = 20.0;
    static float yBuffer = 40.0;

    //Render if in a valid screen position
    if (/*isInFrontOfCam(&pos) && isWithinFrame(&screenPos)*/true) {
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

        if (!isInFrontOfCam(&pos)) {
            if (screenPos.x < (FRAME_SIZE_X/2)) {
                screenPos.x = xBuffer;
            } else {
                screenPos.x = FRAME_SIZE_X-xBuffer;
            }

            //Invert X and Y position
            screenPos.y = FRAME_SIZE_Y - screenPos.y + (yBuffer);
            screenPos.x = FRAME_SIZE_X - screenPos.x;
        }

        //textPrintF(100, playerNotifYOffets[portNr], TopLeft, PLAYER_COLORS[portNr], 1.0f, "%.2f %.2f", screenPos.x, screenPos.y);

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

        if (PLAYER_INVINCIBILITY[portNr]) {
            powerUpCol = &COLOR_INV;
            current = PLAYER_INVINCIBILITY_TIMER[portNr];
            max = gPlayerState.InvincibleTimerMax;
        } else if (PLAYER_SUPERCHARGE[portNr]) {
            powerUpCol = &COLOR_SUP;
            current = PLAYER_SUPERCHARGE_TIMER[portNr];
            max = gPlayerState.SuperchargeTimerMax;
        }

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
}

//main_hook.s | Runs every frame
void MainUpdate() {
    //Setup vtable hooks
    vtable_GUI_PanelItem_v_StateRunning = GUI_PanelItem_v_StateRunning_hookFunc;

    //If the gameloop isn't running, abort
    if (SE_GameLoop_State != 3) {
        return;
    }

    if (NumberOfPlayers() == 1) {
        PLAYER_BREATHS[0] = gPlayerState.CurrentBreath;
    }

    if (isButtonDown(Button_Dpad_Down, 0)) {
        showDebugTimer++;
        if (showDebugTimer == 60) {
            showDebug = !showDebug;
        }
    } else {
        showDebugTimer = 0;
    }

    //If player 1 is holding down dpad left, reset health and restart game.
    if (isButtonDown(Button_Dpad_Left, 0)) {
        restartGameTimer++;
        if (restartGameTimer >= restartGameTimerMax) {
            urghhhImDead();
            PlayerState_RestartGame(&gPlayerState);
        }
        if (restartGameTimer > restartGameTimerMax) {
            restartGameTimer = restartGameTimerMax;
        }
    } else {
        restartGameTimer = 0;
    }

    updatePlayerList();

    //Skip checking for any input if the first player doesn't exist
    //Set a cooldown timer so other players can't join within a short period of the first player joining
    if (players[0] == -1) {
        playerJoinCooldownTimer = 20;
        return;
    } else {
        playerJoinCooldownTimer--;
        if (playerJoinCooldownTimer < 0) { playerJoinCooldownTimer = 0; }
    }

    for (int i = 0; i < 4; i++) {
        if (players[i] == -1) {
            if (isButtonPressed(Button_A, i) && (playerJoinCooldownTimer == 0)) {
                addNewPlayer(i);
            }
        } else {
            if ((i != 0) && isButtonDown(Button_Dpad_Right, i)) {
                player_leave_timers[i]++;
                if (player_leave_timers[i] >= player_leave_timer_max) {
                    player_leave_timers[i] = 0;
                    removePlayer(i);
                }
            } else {
                player_leave_timers[i] = 0;
            }

            if (checkZDoublePress(i)) {
                teleportPlayersToPlayer(i);
            }

            if (isButtonDown(Button_Dpad_Up, i)) {
                player_restore_timers[i]++;
                if (player_restore_timers[i] >= player_restore_timer_max) {
                    player_restore_timers[i] = 0;
                    restorePlayerControl(i);
                }
            } else {
                player_restore_timers[i] = 0;
            }
        }
    }

    return;
}

//draw_hook.s | Runs every frame during the HUD draw loop
//Drawing stuff to the screen should be done here to avoid garbled textures
void DrawUpdate() {
    //If there are 2 or more players, display names above the players.
    if (NumberOfPlayers() > 1) {
        for (int i = 0; i < 4; i++) {
            if (players[i] != -1) {
                DrawPlayerMarker(i);
            }
        }
    }

    for (int i = 0; i < 4; i++) {
        bool alreadyShowing = false;
        
        //JOIN NOTIFICATION
        if (playerJoinedNotifTimers[i] > 0) {
            if (!alreadyShowing) {
                textPrintF(10, playerNotifYOffets[i], TopLeft, PLAYER_COLORS[i], 1.0f, "P%d: Joined!", i+1);
                alreadyShowing = true;
            }
            playerJoinedNotifTimers[i]--;
        }

        //LEAVING NOTIFICATION
        if (player_leave_timers[i] > 20) {
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
                r.w = (float)r.w * ((float)(player_leave_timers[i]-20) / (float)(player_leave_timer_max-20));

                Util_DrawRect(gpPanelWnd, &r, &COLOR_RED);

                alreadyShowing = true;
            }
        }

        //LEAVE NOTIFICATION
        if (playerLeaveNotifTimers[i] > 0) {
            if (!alreadyShowing) {
                textPrintF(10, playerNotifYOffets[i], TopLeft, PLAYER_COLORS[i], 1.0f, "P%d: Left", i+1);
                alreadyShowing = true;
            }
            
            playerLeaveNotifTimers[i]--;
        }

        //MAKING UNSTUCK NOTIFICATION
        if (player_restore_timers[i] > 20) {
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
                r.w = (float)r.w * ((float)(player_restore_timers[i]-20) / (float)(player_restore_timer_max-20));

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
    }

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

    //DEBUG BELOW:
    
    if (!showDebug) { return; }

    textSmpPrintF(20, 100, "tgt: %x", cameraTargetItem);

    for (int i = 0; i < 4; i++) {
        int* handler = ItemEnv_FindUniqueIDHandler(&theItemEnv, players[i], 0);
        textSmpPrintF(20, 120 + (i * 15), "%d: %x | %x", i+1, players[i], handler);
    }

    if (cameraTargetItem != NULL) {
        EXVector* pos = (EXVector*) (cameraTargetItem + (0xD0/4));

        drawSquareAtVec(pos, 4, &COLOR_RED);
    }

    return;
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
    } else if (vtable == SPARX_VTABLE) {
        SparxPreUpdate(self);
    } else if (vtable == LOCKEDCHEST_VTABLE) {
        SetPlayerRefToPort(0);
    } else    
    {
        MiscHandlerPreUpdate(self);
    }

    //This is only reached if each case doesn't return on its own
    bool res = ItemHandler_SEUpdate(self);

    //Return control to player 1 after updating
    g_PadNum = 0;
    return res;
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

//Runs after the "urghhhImDead" function for Spyro, Blink and Hunter.
void urghhhImDead() {
    gPlayerState.Health = 0xA0;
    PLAYER_HEALTH[0] = 0xA0;
    PLAYER_HEALTH[1] = 0xA0;
    PLAYER_HEALTH[2] = 0xA0;
    PLAYER_HEALTH[3] = 0xA0;
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