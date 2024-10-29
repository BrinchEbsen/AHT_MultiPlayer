#include <common.h>
#include <hashcodes.h>
#include <symbols.h>
#include <main.h>

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