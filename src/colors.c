#include <common.h>

//Colors

//                         R     G     B     A
XRGBA COLOR_TEXT        = {0x7F, 0x6C, 0x36, 0x80};
XRGBA COLOR_WHITE       = {0x80, 0x80, 0x80, 0x80};
XRGBA COLOR_BLACK       = {0   , 0   , 0   , 0x80};
XRGBA COLOR_RED         = {0x80, 0,    0,    0x80};
XRGBA COLOR_GREEN       = {0   , 0x80, 0,    0x80};
XRGBA COLOR_BLUE        = {0   , 0,    0x80, 0x80};
XRGBA COLOR_LIGHT_RED   = {0x80, 0x40, 0x40, 0x80};
XRGBA COLOR_LIGHT_GREEN = {0x40, 0x80, 0x40, 0x80};
XRGBA COLOR_LIGHT_BLUE  = {0x40, 0x40, 0x80, 0x80};

XRGBA COLOR_BLANK = {0, 0, 0, 0};

XRGBA COLOR_P1 = {0x00, 0x48, 0x80, 0x80}; //Blue
XRGBA COLOR_P2 = {0x80, 0x20, 0x20, 0x80}; //Red
XRGBA COLOR_P3 = {0x08, 0x60, 0x00, 0x80}; //Green
XRGBA COLOR_P4 = {0x80, 0x50, 0x00, 0x80}; //Yellow

XRGBA COLOR_INV = {0x40, 0x40, 0x80, 0x80};
XRGBA COLOR_SUP = {0x80, 0x40, 0x40, 0x80};

int XRGBA_Luminance(XRGBA* col) {
    int r = col->r;
    int g = col->g;
    int b = col->b;

    return (r+r+r+b+g+g+g+g)>>3;
}

XRGBA* XRGBA_Balance(XRGBA* col, int bal) {
    int lum = XRGBA_Luminance(col);
    float ratio = (float)bal/(float)lum;

    col->r = (int)(col->r * ratio);
    col->g = (int)(col->g * ratio);
    col->b = (int)(col->b * ratio);

    return col;
}