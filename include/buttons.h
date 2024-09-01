#ifndef BUTTONS_H
#define BUTTONS_H
#include <custom_types.h>
#include <symbols.h>
#include <common.h>

//Check if the button is held down
bool isButtonDown(Buttons button, int padNum);
//Check if the button was held down on the last frame
bool isButtonLast(Buttons button, int padNum);
//Check if the button has been pressed
bool isButtonPressed(Buttons button, int padNum);
//Check if the button has been released
bool isButtonReleased(Buttons button, int padNum);

/*
Visual example of a possible sequence of frames.
"x" denotes when a function returns true.

B button held:     ---BBBBBB----

isButtonDown:      ---xxxxxx----
isButtonLast:      ----xxxxxx---
isButtonPressed:   ---x---------
isButtonReleased:  ---------x---
*/

#endif /* BUTTONS_H */