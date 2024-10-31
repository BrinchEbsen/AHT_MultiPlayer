#include <main.h>
#include <loadingscreen.h>
#include <players.h>

bool MOD_INIT = false;

int gMap_MechaRed = 0x8045b5d8;
int gLevel_VolcanoDescent2 = 0x8045e480;

DeathMode deathMode = PlayerDespawn;

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

bool initialized = false;
bool showDebug = false;
int showDebugTimer = 0;
bool doMultiplayerOptions = false;
bool showMP_Notifs = false;

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
int lastPlayerUpdated = 0;
int globalRefPortNr = 0;
int playerRestoreTimers[4] = {0, 0, 0, 0};
int playerRestoreTimerMax = 60;
int playerRestoreNotifTimers[] = {0, 0, 0, 0};
int playerLeaveTimers[4] = {0, 0, 0, 0};
int playerLeaveTimerMax = 90;
bool leaveBecauseDeath[] = {false, false, false, false};
int playerLeaveNotifTimers[] = {0, 0, 0, 0};
int playerJoinCooldownTimers[] = {0, 0, 0, 0};
int playerJoinCooldownTimerMax = PLAYER_JOIN_COOLDOWN_MAX_DEFAULT;
bool showCoolDownTimer = false;

//Current player handler being tracked for doing a breath attack
int* currentBreather = NULL;

void SetupVtableHooks() {
    vtable_GUI_PanelItem_v_StateRunning = GUI_PanelItem_v_StateRunning_Hook;
    vtable_GUI_PauseMenu_v_DrawStateRunning = GUI_PauseMenu_v_DrawStateRunning_Hook;
    vtable_XGameWnd_Draw = XGameWnd_Draw_Hook;
    vtable_LoadingLoopDraw = LoadingLoopDraw_ReImpl;
}

inline bool isPointer(void* ptr) {
    if ((ptr < 0x817FFFFF) && (ptr >= 0x80000000)) { return true; }
    return false;
}

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

void EXVector_Copy(EXVector* dest, EXVector* src) {
    dest->x = src->x;
    dest->y = src->y;
    dest->z = src->z;
    dest->w = src->w;
}

EXRuntimeClass* GetHandlerRuntimeClass(int* handler) {
    int* vtable = (int*) *(handler + (0x4/4));
    GetRuntimeClass_func getRTC = (GetRuntimeClass_func) *(vtable + (0x8/4));
    return getRTC();
}

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

bool GameIsPaused() {
    int flags = *((&gGameLoop) + (0x88/4));
    return (flags & 0x80000000) != 0;
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

SE_Map_v_PlayerSetup GetMapPlayerSetupFunc(int* map) {
    int* vtable = (int*) *(map + (0x74/4));

    return *(vtable + (0xc0/4));
}

ItemHandler_Delete GetHandlerDeleteFunc(int* handler) {
    int* vtable = (int*) *(handler + (0x4/4));

    return *(vtable + (0x50/4));
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

void updateCameraTargetItem() {
    if (!itemExists(cameraTargetItem)) {
        //Construct camera target item
        cameraTargetItem = XSEItem_CreateObject();
        //add to iten environment
        XSEItemEnv_AddXSEItem(&theItemEnv, cameraTargetItem, 0);
        //ig_printf("Creating new item for camera focus\n");
    }
}

void PlayerHandlerPreUpdate(int* self) {
    gpPlayer = self;
    gpPlayerItem = (int*) *self;

    int ID = *(self + 0x8/4);

    for (int i = 0; i < 4; i++) {
        if (players[i] == ID) {
            g_PadNum = i;
            globalRefPortNr = i;
            lastPlayerUpdated = i;

            //if (!OnlyPlayer1Exists()) {
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
            //}
        }
    }

    //EXODListItem* breath = (EXODListItem*)(self + (0x3bc/4));
    //if (!EXODList_IsMember(&gpBreath, breath)) {
    //    breath->next = NULL;
    //    breath->prev = NULL;
    //    ItemHandler_AddToBreath(self, 1);
    //}
}

void PlayerHandlerPostUpdate(int* self) {
    //After the update, we store the playerstate globals that resulted from the update.
    //if (OnlyPlayer1Exists()) { return; }

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

    return;

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

void MainUpdate() {
    if (!MOD_INIT) {
        MOD_INIT = true;

        SetupVtableHooks();
    }

    for (int i = 0; i < CHARACTER_AMOUNT; i++) {
        *((int*)(CHARACTER_VTABLES[i] + 0xBC)) = VtableSwap_Player_TestBreathType;
    }

    //If the gameloop isn't running, abort
    if ((SE_GameLoop_State != 3) || GameIsPaused()) {
        restartGameTimer = 0;
        for (int i = 0; i < 4; i++) {
            playerLeaveTimers[i] = 0;
            playerRestoreTimers[i] = 0;
        }

        showMP_Notifs = false;

        return;
    } else {
        showMP_Notifs = true;
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

void DrawUpdate() {
    //Draw markers for the players if player 1 isn't alone.
    for (int i = 0; i < 4; i++) {
        if (players[i] != -1) {
            DrawPlayerMarker(i);
        }
    }

    //Show notif saying Sparx can't be played in multiplayer
    if ((NumberOfPlayers() == 0) && (showMP_Notifs)) {
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
    if (showMP_Notifs) {
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
    }
    
    //Draw restart game timer
    if ((restartGameTimer > 20) && (showMP_Notifs)) {
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

bool ScanUpdate() {
    //return isButtonPressed(Button_Z, g_PadNum);
    return false;
}

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

void Sparx_SetPlayerHealth_Hook(int* self, int health) {
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
}

void Butterfly_Special_SetHealth_Hook(int* self, int health) {
    SetAllHealthFull();
}

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

bool VtableSwap_Player_TestBreathType(int* self, BreathType type) {
    return false; //temporary

    ig_printf("self: %x, currentBreather: %x\n", self, currentBreather);

    if ((currentBreather == NULL) || (currentBreather == self)) {
        return false;
    }
    
    return (type & 1) != 0;
}

void ReImpl_SpyroHunter_urghhhImDead(int* self) {
    if (handlerIsOnlyPlayerLeft(self) || (deathMode == ReloadGame)) {
        SetAllHealthFull();
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

void ReImpl_Blink_urghhhImDead(int* self) {
    int portNr = GetPortNrFromPlayerHandler(self);

    if (handlerIsOnlyPlayerLeft(self) || (deathMode == ReloadGame)) {
        int* map = PlayerSetupInfo_GetMap(&gPlayerState.Setup);

        //Reset health
        SetAllHealthFull();

        if (MapIsMinigame(map)) {
            int* vtable = (int*) *(map + (0x74/4));
            SE_Map_SetMiniGameDie_func setDieFunc = (SE_Map_SetMiniGameDie_func) *(vtable + (0xf8/4));
            setDieFunc(map);

            //Set some flags in the animator
            int* anim = (int*) *(self + (0x144/4));
            if (isPointer(anim)) {
                ushort* itemFlags = (ushort*) (anim + (0xc/4));
                *itemFlags &= ~((ushort) 1);
            }

            //Set some flags in the player handler
            int* playerStateFlags = self + (0x580/4);
            *playerStateFlags |= 2; //ps_dead
        } else {
            //If it's not a minigame level, just do the usual stuff
            PlayerState_RestartGame(&gPlayerState);
        }
    } else {
        gPlayerState.Health = 0xA0;
        PLAYER_HEALTH[portNr] = 0xA0;

        playerJoinCooldownTimers[portNr] = playerJoinCooldownTimerMax;
        showCoolDownTimer = true;
        removePlayer(portNr, true);
    }
}

void ReImpl_SgtByrd_urghhhImDead(int* self) {
    if (handlerIsOnlyPlayerLeft(self) || (deathMode == ReloadGame)) {
        int* map = PlayerSetupInfo_GetMap(&gPlayerState.Setup);

        //Reset health
        SetAllHealthFull();

        if (MapIsMinigame(map)) {
            int* vtable = (int*) *(map + (0x74/4));
            SE_Map_SetMiniGameDie_func setDieFunc = (SE_Map_SetMiniGameDie_func) *(vtable + (0xf8/4));
            setDieFunc(map);

            //Set some flags in the player handler
            int* playerStateFlags = self + (0x580/4);
            *playerStateFlags |= 2; //ps_dead
        } else {
            //If it's not a minigame level, just do the usual stuff
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

int GUI_PanelItem_v_StateRunning_Hook(int* self) {
    //Normal behavior if singleplayer
    //if (NumberOfPlayers() <= 1) { return GUI_PanelItem_v_StateRunning(self); }
    
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

bool XGameWnd_Draw_Hook(int* self) {
    return XGameWnd_Draw(self);
}

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

bool Particle_Fire_FlameObjects_Hook(int* self, int breath, EXVector* pos, float dist) {
    currentBreather = self;

    //ig_printf("set currentBreather\n");

    bool res = Player_FlameObjects(self, breath, pos, dist);

    currentBreather = NULL;

    //ig_printf("reset currentBreather\n");

    return res;
}

bool ElecBreathStop_PlaySFX_NullCheckFix_Hook(uint soundHash, int* item) {
    if (gpPlayer == NULL) { return false; }

    return PlaySFX_AtItem(soundHash, (int*) *gpPlayer);
}