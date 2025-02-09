#ifndef __GAME_LUA_H__
#define __GAME_LUA_H__

#include <LuaWrapper.h>

#include "constants.h"
#include "hardware/gfxHandler.hpp"
#include "hardware/sdHandler.h"
#include "hardware/touchHandler.h"
#include "hardware/prefsHandler.h"

void luaDemoTest();

void initLuaBindings();
void initGames_lua(String gamePath, GameConfig* gameConfig, String* errorMessage);
void drawShortBlowGame_lua(DISPLAY_T* display, BlowData* blowData, String* errorMessage);
void drawEqualBlowGame_lua(DISPLAY_T* display, BlowData* blowData, String* errorMessage);
void drawLongBlowGame_lua(DISPLAY_T* display, BlowData* blowData, String* errorMessage);
void drawTrampolineGame_lua(DISPLAY_T* display, JumpData* jumpData, String* errorMessage);
void drawInhalationGame_lua(DISPLAY_T* display, BlowData* blowData, String* errorMessage);
bool displayProgressionMenu_lua(DISPLAY_T *display, String *errorMessage);

extern bool luaProgressionMenuRunning;
extern DISPLAY_T* luaDisplay;
extern LuaWrapper lua;

#endif /* __GAME_LUA_H__ */