#include <common.h>
#include <symbols.h>

const u16 FRAME_SIZE_X = 512;
const u16 FRAME_SIZE_Y = 448;

bool isInFrontOfCam(EXVector* vct) {
    EXVector* camPos = &(CamMatrix.row3);
    
    EXVector v = {
        vct->x - camPos->x,
        vct->y - camPos->y,
        vct->z - camPos->z,
        0.0f
    };

    EXVector* camFacing = &(CamMatrix.row2);

    float dot = camFacing->x*v.x + camFacing->y*v.y + camFacing->z*v.z;

    return dot > 0.0f;
}

bool isWithinFrame(EXVector2* vct) {
    return
    (vct->x >= 0.0f) &&
    (vct->x <= (float) FRAME_SIZE_X) &&
    (vct->y >= 0.0f) &&
    (vct->y <= (float) FRAME_SIZE_Y);
}

void drawSquareAtVec(EXVector* vct, int size, XRGBA* col) {
    if (!isInFrontOfCam(vct)) {
        return;
    }

    EXVector2 scrVect = {0.0f, 0.0f};

    WorldToDisp(&scrVect, vct);

    if (isWithinFrame(&scrVect)) {
        EXRect r = {
            (int) scrVect.x -(size/2),
            (int) scrVect.y -(size/2),
            size,
            size
        };
        Util_DrawRect(gpPanelWnd, &r, col);
    }
}