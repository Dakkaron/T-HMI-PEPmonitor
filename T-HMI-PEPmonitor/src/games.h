#ifndef __GAMES_H__
#define __GAMES_H__

#include <Arduino.h>
#include "gfxHandler.hpp"
#include "sdHandler.h"

void initGames(String gamePath, String* errorMessage);
void drawShortBlowGame(DISPLAY_T* display, BlowData* blowData, String* errorMessage);
void drawLongBlowGame(DISPLAY_T* display, BlowData* blowData, String* errorMessage);
void drawTrampolineGame(DISPLAY_T* display, JumpData* jumpData, String* errorMessage);
void displayProgressionMenu(DISPLAY_T* display, String* errorMessage);
String getRandomWinScreenPathForCurrentGame(String* errorMessage);

#endif /* __GAMES_H__ */