#include <players.h>
#include <colors.h>
#include <symbols.h>
#include <main.h>

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

int players[4] = {-1, -1, -1, -1};

int NumberOfPlayers() {
    int count = 0;

    for (int i = 0; i < 4; i++) {
        if (players[i] != -1) {
            count++;
        }
    }

    return count;
}

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

bool OnlyPlayer1Exists() {
    return (players[0] != -1) && (NumberOfPlayers() == 1);
}

bool handlerIsOnlyPlayerLeft(int* handler) {
    int ID = *(handler + (0x8/4));

    for (int i = 0; i < 4; i++) {
        if (players[i] == ID) { continue; } //ignore self

        if (players[i] != -1) { return false; }
    }

    return true;
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
    globalRefPortNr = portNr;

    return true;
}

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

void teleportPlayer(int* player, EXVector* dest) {
    if (player == NULL) { return; }

    EXVector* lastPos = (EXVector*) (player + (0x8c0 / 4));
    EXVector_Copy(lastPos, dest);

    int* item = (int*) *player;
    EXVector* pos = (EXVector*) (item + (0xd0 / 4));
    EXVector_Copy(pos, dest);

    int* itemPhys = (int*) *(item + (0x150 / 4));
    if (itemPhys == NULL) { return; }

    EXVector* prevPos = (EXVector*) (itemPhys + (0x50 / 4));
    EXVector* oldPos = (EXVector*) (itemPhys + (0x148 / 4));
    EXVector* collIntersect = (EXVector*) (itemPhys + (0x138 / 4));

    EXVector_Copy(prevPos, dest);
    EXVector_Copy(oldPos, dest);
    EXVector_Copy(collIntersect, dest);
}

void teleportPlayersToPlayer(int portNr) {
    if (players[portNr] == -1) { return; }

    int* handler = ItemEnv_FindUniqueIDHandler(&theItemEnv, players[portNr], 0);
    if (handler == NULL) { return; }

    int* list[4];
    int count = GetArrayOfPlayerHandlers(&list);
    if (count < 2) { return; } //Ignore if only 1 player

    for (int i = 0; i < count; i++) {
        if (handler == list[i]) { continue; }

        teleportPlayer(list[i], handler);
    }
}

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

void SetAllHealthFull() {
    gPlayerState.Health = 0xA0;
    for (int i = 0; i < 4; i++) {
        PLAYER_HEALTH[i] = 0xA0;
    }
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