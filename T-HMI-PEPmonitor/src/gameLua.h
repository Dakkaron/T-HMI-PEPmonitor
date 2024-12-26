#ifndef __GAME_LUA_H__
#define __GAME_LUA_H__

#include "config.h"
#include "constants.h"
#include "EEPROM.h"
#include "gfxHandler.hpp"
#include "sdHandler.h"
#include "touchHandler.h"
#include "prefsHandler.h"

void luaDemoTest();

void initGames_lua(String gamePath, GameConfig* gameConfig, String* errorMessage);
void drawShortBlowGame_lua(DISPLAY_T* display, BlowData* blowData, String* errorMessage);
void drawLongBlowGame_lua(DISPLAY_T* display, BlowData* blowData, String* errorMessage);
void drawTrampolineGame_lua(DISPLAY_T* display, JumpData* jumpData, String* errorMessage);
void displayProgressionMenu_lua(DISPLAY_T *display, String *errorMessage);

#endif /* __GAME_LUA_H__ */