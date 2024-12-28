#ifndef __PHYSIO_PROTOCOL_HANDLER_H__
#define __PHYSIO_PROTOCOL_HANDLER_H__
#include <Arduino.h>

void runProfileSelection(String* errorMessage);
void handlePhysioTask();
void displayPhysioRotateScreen();

#endif