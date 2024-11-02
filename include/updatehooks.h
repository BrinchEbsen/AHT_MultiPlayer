#ifndef UPDATEHOOKS_H
#define UPDATEHOOKS_H
#include <common.h>

/// @brief Branch-link hook at 0x8023dfec:
/// Replaces call to XSEItemHandler::SEUpdate to do updates
/// to global data before/after the handler updates.
/// @param self Handler this-pointer.
bool ItemHandler_SEUpdate_Hook(int* self);

#endif