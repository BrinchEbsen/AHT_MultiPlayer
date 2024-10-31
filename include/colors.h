#ifndef COLORS_H
#define COLORS_H
#include <common.h>

extern XRGBA COLOR_TEXT;
extern XRGBA COLOR_WHITE;
extern XRGBA COLOR_BLACK;
extern XRGBA COLOR_RED;
extern XRGBA COLOR_GREEN;
extern XRGBA COLOR_BLUE;
extern XRGBA COLOR_LIGHT_RED;
extern XRGBA COLOR_LIGHT_GREEN;
extern XRGBA COLOR_LIGHT_BLUE;

extern XRGBA COLOR_BLANK;

extern XRGBA COLOR_P1; //Blue
extern XRGBA COLOR_P2; //Red
extern XRGBA COLOR_P3; //Green
extern XRGBA COLOR_P4; //Yellow

extern XRGBA COLOR_INV;
extern XRGBA COLOR_SUP;

//Approximate luminance from RGB
int XRGBA_Luminance(XRGBA* col);

//Balance RGB values to match given luminance
XRGBA* XRGBA_Balance(XRGBA* col, int bal);

#endif