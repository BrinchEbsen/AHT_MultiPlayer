#include <updatehooks.h>
#include <players.h>
#include <main.h>

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