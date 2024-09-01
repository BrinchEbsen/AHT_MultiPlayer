#ifndef TEXTPRINT_H
#define TEXTPRINT_H
#include <custom_types.h>
#include <common.h>

//Draws text to the screen with a dropshadow to emulate the other HUD text in the game.
void textPrint(char* pText, wchar_t* pTextW, u16 x, u16 y, TextAlign Align, XRGBA* textCol, float Scale);
//Simplified version of textPrint
void textSmpPrint(char* pText, wchar_t* pTextW, u16 x, u16 y);
//Print a formatted string to the screen
void textPrintF(u16 x, u16 y, TextAlign Align, XRGBA* textCol, float Scale, char* pText, ...);
//Simplified version of textPrintF
void textSmpPrintF(u16 x, u16 y, char* pText, ...);

#endif /* TEXTPRINT_H */