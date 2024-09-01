#ifndef INPUTDISPLAY_H
#define INPUTDISPLAY_H
#include <common.h>

void drawInputButton(EXRect* r, Buttons b, int padNum);
void drawInputTrigger(EXRect* r, float amount);
void drawInputStick(EXRect* r, float x, float y);
void drawInputVis(u16 x, u16 y, int padNum);

#endif /* INPUTDISPLAY_H */