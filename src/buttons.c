#include <buttons.h>
#include <symbols.h>

bool isButtonDown(Buttons button, int padNum) {
    return (Pads_ButtonDown[padNum] & button) != 0;
}
bool isButtonLast(Buttons button, int padNum) {
    return (Pads_ButtonLast[padNum] & button) != 0;
}
bool isButtonPressed(Buttons button, int padNum) {
    return (Pads_ButtonPressed[padNum] & button) != 0;
}
bool isButtonReleased(Buttons button, int padNum) {
    return (Pads_ButtonRelease[padNum] & button) != 0;
}