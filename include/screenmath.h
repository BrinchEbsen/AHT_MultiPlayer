#ifndef SCREENMATH_H
#define SCREENMATH_H
#include <common.h>

//Test whether a 3D vector exists in front of the game camera.
bool isInFrontOfCam(EXVector* vct);
//Test whether a 2D vector exists within the rendered frame.
bool isWithinFrame(EXVector2* vct);

//Draw a square at a specificed world position.
void drawSquareAtVec(EXVector* vct, int size, XRGBA* col);

#endif /* SCREENMATH_H */