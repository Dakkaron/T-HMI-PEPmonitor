#include "gameLua.h"
#include <LuaWrapper.h>
#include "hardware/sdHandler.h"
#include "hardware/gfxHandler.hpp"

LuaWrapper lua;
static String luaGamePath;

static String longBlowScript;
static String shortBlowScript;
static String equalBlowScript;
static String trampolineScript;
static String progressionMenuScript;

static DISPLAY_T* currentDisplay;

#define SPRITE_COUNT_LIMIT 50

// Block of 50 sprites to be used within Lua games
static TFT_eSprite sprites[] = {TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),
                                TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),
                                TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),
                                TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),
                                TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),
                                TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),
                                TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),
                                TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),
                                TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),
                                TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),};

static int lua_wrapper_loadBmp(lua_State* luaState) {
  Serial.println("Load BMP");
  String path = luaGamePath + luaL_checkstring(luaState, 1);
  int32_t flipped = luaL_optinteger(luaState, 2, 0);
  for (int32_t i=0;i<SPRITE_COUNT_LIMIT;i++) {
    if (!sprites[i].created()) {
      loadBmp(&sprites[i], path, flipped);
      Serial.print("Found sprite slot: ");
      Serial.println(i);
      lua_pushinteger(luaState, i);
      return 1;
    }
  }
  Serial.println("Failed to find sprite slot");
  lua_pushinteger(luaState, -1);
  return 1;
}

static int lua_wrapper_drawSprite(lua_State* luaState) {
  Serial.println("Draw sprite");
  int16_t handle = luaL_checkinteger(luaState, 1);
  int16_t x = luaL_checknumber(luaState, 2);
  int16_t y = luaL_checknumber(luaState, 3);
  sprites[handle].pushToSprite(currentDisplay, x, y);
  Serial.println("Draw sprite done");
  return 0;
}

static int lua_wrapper_drawString(lua_State* luaState) {
  String s = luaL_checkstring(luaState, 1);
  int16_t x = luaL_checknumber(luaState, 2);
  int16_t y = luaL_checknumber(luaState, 3);
  currentDisplay->drawString(s, x, y);
  return 0;
}


void initGames_lua(String gamePath, GameConfig* gameConfig, String* errorMessage) {
  luaGamePath = gamePath;
  lua.Lua_register("loadBmp", (const lua_CFunction) &lua_wrapper_loadBmp);
  lua.Lua_register("drawSprite", (const lua_CFunction) &lua_wrapper_drawSprite);
  lua.Lua_register("drawString", (const lua_CFunction) &lua_wrapper_drawString);
  String initScript = readFileToString((gamePath + "init.lua").c_str());
  longBlowScript = readFileToString((gamePath + "longBlow.lua").c_str());
  shortBlowScript = readFileToString((gamePath + "shortBlow.lua").c_str());
  equalBlowScript = readFileToString((gamePath + "equalBlow.lua").c_str());
  trampolineScript = readFileToString((gamePath + "trampoline.lua").c_str());
  progressionMenuScript = readFileToString((gamePath + "progressionMenu.lua").c_str());
  if (!initScript.isEmpty()) {
    lua.Lua_dostring(&initScript);
  }
}

void updateBlowData(BlowData* blowData) {
  String blowDataString = "currentlyBlowing="+String(blowData->currentlyBlowing)+"\n"+\
                          "ms="+String(blowData->ms)+"\n"+\
                          "blowStartMs="+String(blowData->blowStartMs)+"\n"+\
                          "blowEndMs="+String(blowData->blowEndMs)+"\n"+\
                          "targetDurationMs="+String(blowData->targetDurationMs)+"\n"+\
                          "cycleNumber="+String(blowData->cycleNumber)+"\n"+\
                          "totalCycleNumber="+String(blowData->totalCycleNumber)+"\n"+\
                          "blowCount="+String(blowData->blowCount)+"\n"+\
                          "totalBlowCount="+String(blowData->totalBlowCount)+"\n"+\
                          "pressure="+String(blowData->pressure)+"\n"+\
                          "peakPressure="+String(blowData->peakPressure)+"\n"+\
                          "minPressure="+String(blowData->minPressure)+"\n"+\
                          "targetPressure="+String(blowData->targetPressure)+"\n"+\
                          "cumulativeError="+String(blowData->cumulativeError)+"\n"+\
                          "fails="+String(blowData->fails)+"\n"+\
                          "taskType="+String(blowData->taskType)+"\n"+\
                          "lastBlowStatus="+String(blowData->lastBlowStatus);
  lua.Lua_dostring(&blowDataString);
}

void updateJumpData(JumpData* jumpData) {
  String jumpDataString = "ms = "+jumpData->ms;
  lua.Lua_dostring(&jumpDataString);
}

void drawShortBlowGame_lua(DISPLAY_T* display, BlowData* blowData, String* errorMessage) {
  currentDisplay = display;
  updateBlowData(blowData);
  lua.Lua_dostring(&shortBlowScript);
}

void drawLongBlowGame_lua(DISPLAY_T* display, BlowData* blowData, String* errorMessage) {
  currentDisplay = display;
  updateBlowData(blowData);
  lua.Lua_dostring(&longBlowScript);
}

void drawEqualBlowGame_lua(DISPLAY_T* display, BlowData* blowData, String* errorMessage) {
  currentDisplay = display;
  updateBlowData(blowData);
  lua.Lua_dostring(&equalBlowScript);
}

void drawTrampolineGame_lua(DISPLAY_T* display, JumpData* jumpData, String* errorMessage) {
  currentDisplay = display;
  updateJumpData(jumpData);
  lua.Lua_dostring(&trampolineScript);
}

void displayProgressionMenu_lua(DISPLAY_T *display, String *errorMessage) {
  currentDisplay = display;
  lua.Lua_dostring(&progressionMenuScript);
}