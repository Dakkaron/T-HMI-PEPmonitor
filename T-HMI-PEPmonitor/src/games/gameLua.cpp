#include "gameLua.h"
#include "hardware/sdHandler.h"
#include "hardware/gfxHandler.hpp"
#include "hardware/prefsHandler.h"

LuaWrapper lua;
static String luaGamePath;

DISPLAY_T* luaDisplay;
bool luaProgressionMenuRunning;

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
  int16_t transp = luaL_optinteger(luaState, 4, 0x0000);
  sprites[handle].pushToSprite(luaDisplay, x, y, transp);
  Serial.println("Draw sprite done");
  return 0;
}

static int lua_wrapper_drawSpriteRegion(lua_State* luaState) {
  Serial.println("Draw sprite region");
  int16_t handle = luaL_checkinteger(luaState, 1);
  int16_t tx = luaL_checknumber(luaState, 2);
  int16_t ty = luaL_checknumber(luaState, 3);
  int16_t sx = luaL_checknumber(luaState, 4);
  int16_t sy = luaL_checknumber(luaState, 5);
  int16_t sw = luaL_checknumber(luaState, 6);
  int16_t sh = luaL_checknumber(luaState, 7);
  int16_t transp = luaL_optinteger(luaState, 8, 0x0000);
  sprites[handle].pushToSprite(luaDisplay, tx, ty, sx, sy, sw, sh, transp);
  Serial.println("Draw sprite region done");
  return 0;
}

static int lua_wrapper_drawAnimSprite(lua_State* luaState) {
  Serial.println("Draw anim sprite X");
  int16_t handle = luaL_checkinteger(luaState, 1);
  int16_t tx = luaL_checknumber(luaState, 2);
  int16_t ty = luaL_checknumber(luaState, 3);
  int16_t sw = luaL_checknumber(luaState, 4);
  int16_t sh = luaL_checknumber(luaState, 5);
  int16_t frame = luaL_checknumber(luaState, 6);
  int16_t transp = luaL_optnumber(luaState, 7, 0x0000);
  
  int16_t cols = sprites[handle].width() / sw;
  int16_t rows = sprites[handle].height() / sh;

  int16_t col = frame % cols;
  int16_t row = frame / cols;

  sprites[handle].pushToSprite(luaDisplay, tx, ty, sw*col, sh*row, sw, sh, transp);
  Serial.println("Draw anim sprite done");
  return 0;
}

static int lua_wrapper_log(lua_State* luaState) {
  String s = luaL_checkstring(luaState, 1);
  Serial.println(s);
  return 0;
}

static int lua_wrapper_drawString(lua_State* luaState) {
  String s = luaL_checkstring(luaState, 1);
  int16_t x = luaL_checknumber(luaState, 2);
  int16_t y = luaL_checknumber(luaState, 3);
  luaDisplay->drawString(s, x, y);
  return 0;
}

static int lua_wrapper_drawRect(lua_State* luaState) {
  int16_t x = luaL_checknumber(luaState, 1);
  int16_t y = luaL_checknumber(luaState, 2);
  int16_t w = luaL_checknumber(luaState, 3);
  int16_t h = luaL_checknumber(luaState, 4);
  uint16_t color = luaL_checknumber(luaState, 5);
  luaDisplay->drawRect(x, y, w, h, color);
  return 0;
}

static int lua_wrapper_fillRect(lua_State* luaState) {
  int16_t x = luaL_checknumber(luaState, 1);
  int16_t y = luaL_checknumber(luaState, 2);
  int16_t w = luaL_checknumber(luaState, 3);
  int16_t h = luaL_checknumber(luaState, 4);
  uint16_t color = luaL_checknumber(luaState, 5);
  luaDisplay->fillRect(x, y, w, h, color);
  return 0;
}

static int lua_wrapper_drawCircle(lua_State* luaState) {
  int16_t x = luaL_checknumber(luaState, 1);
  int16_t y = luaL_checknumber(luaState, 2);
  int16_t r = luaL_checknumber(luaState, 3);
  uint16_t color = luaL_checknumber(luaState, 4);
  luaDisplay->drawCircle(x, y, r, color);
  return 0;
}

static int lua_wrapper_fillCircle(lua_State* luaState) {
  int16_t x = luaL_checknumber(luaState, 1);
  int16_t y = luaL_checknumber(luaState, 2);
  int16_t r = luaL_checknumber(luaState, 3);
  uint16_t color = luaL_checknumber(luaState, 4);
  luaDisplay->fillCircle(x, y, r, color);
  return 0;
}

static int lua_wrapper_drawLine(lua_State* luaState) {
  int16_t x0 = luaL_checknumber(luaState, 1);
  int16_t y0 = luaL_checknumber(luaState, 2);
  int16_t x1 = luaL_checknumber(luaState, 3);
  int16_t y1 = luaL_checknumber(luaState, 4);
  uint16_t color = luaL_checknumber(luaState, 5);
  luaDisplay->drawLine(x0, y0, x1, y1, color);
  return 0;
}

static int lua_wrapper_drawFastHLine(lua_State* luaState) {
  int16_t x = luaL_checknumber(luaState, 1);
  int16_t y = luaL_checknumber(luaState, 2);
  int16_t w = luaL_checknumber(luaState, 3);
  uint16_t color = luaL_checknumber(luaState, 4);
  luaDisplay->drawFastHLine(x, y, w, color);
  return 0;
}

static int lua_wrapper_drawFastWLine(lua_State* luaState) {
  int16_t x = luaL_checknumber(luaState, 1);
  int16_t y = luaL_checknumber(luaState, 2);
  int16_t h = luaL_checknumber(luaState, 3);
  uint16_t color = luaL_checknumber(luaState, 4);
  luaDisplay->drawFastVLine(x, y, h, color);
  return 0;
}

static int lua_wrapper_fillScreen(lua_State* luaState) {
  uint16_t color = luaL_checknumber(luaState, 1);
  luaDisplay->fillScreen(color);
  return 0;
}

static int lua_wrapper_setTextColor(lua_State* luaState) {
  uint16_t color = luaL_checknumber(luaState, 1);
  luaDisplay->setTextColor(color);
  return 0;
}

static int lua_wrapper_setTextSize(lua_State* luaState) {
  uint8_t size = luaL_checknumber(luaState, 1);
  luaDisplay->setTextSize(size);
  return 0;
}

static int lua_wrapper_setTextDatum(lua_State* luaState) {
  uint8_t datum = luaL_checknumber(luaState, 1);
  luaDisplay->setTextDatum(datum);
  return 0;
}

static int lua_wrapper_setCursor(lua_State* luaState) {
  int16_t x = luaL_checknumber(luaState, 1);
  int16_t y = luaL_checknumber(luaState, 2);
  luaDisplay->setCursor(x, y);
  return 0;
}

static int lua_wrapper_print(lua_State* luaState) {
  String s = luaL_checkstring(luaState, 1);
  luaDisplay->print(s);
  return 0;
}

static int lua_wrapper_println(lua_State* luaState) {
  String s = luaL_checkstring(luaState, 1);
  luaDisplay->println(s);
  return 0;
}

static int lua_wrapper_cls(lua_State* luaState) {
  luaDisplay->fillSprite(TFT_BLACK);
  return 0;
}

static int lua_wrapper_prefsSetString(lua_State* luaState) {
  String key = luaL_checkstring(luaState, 1);
  String value = luaL_checkstring(luaState, 2);
  prefs.putString(key.c_str(), value.c_str());
  return 0;
}

static int lua_wrapper_prefsGetString(lua_State* luaState) {
  String key = luaL_checkstring(luaState, 1);
  String def = luaL_optstring(luaState, 2, "");
  String value = prefs.getString(key.c_str(), def.c_str());
  lua_pushstring(luaState, value.c_str());
  return 1;
}

static int lua_wrapper_prefsSetInt(lua_State* luaState) {
  String key = luaL_checkstring(luaState, 1);
  int value = luaL_checkinteger(luaState, 2);
  prefs.putInt(key.c_str(), value);
  return 0;
}

static int lua_wrapper_prefsGetInt(lua_State* luaState) {
  String key = luaL_checkstring(luaState, 1);
  int def = luaL_optinteger(luaState, 2, 0);
  int value = prefs.getInt(key.c_str(), def);
  lua_pushinteger(luaState, value);
  return 1;
}

static int lua_wrapper_prefsSetNumber(lua_State* luaState) {
  String key = luaL_checkstring(luaState, 1);
  float value = luaL_checknumber(luaState, 2);
  prefs.putFloat(key.c_str(), value);
  return 0;
}

static int lua_wrapper_prefsGetNumber(lua_State* luaState) {
  String key = luaL_checkstring(luaState, 1);
  float def = luaL_optnumber(luaState, 2, 0.0);
  float value = prefs.getFloat(key.c_str(), def);
  lua_pushnumber(luaState, value);
  return 1;
}

static int lua_wrapper_closeProgressionMenu(lua_State* luaState) {
  luaProgressionMenuRunning = false;
  return 0;
}

void initLuaBindings() {
  static bool bindingsInitiated = false;
  if (bindingsInitiated) {
    return;
  }
  lua.Lua_register("loadBmp", (const lua_CFunction) &lua_wrapper_loadBmp);
  lua.Lua_register("drawSprite", (const lua_CFunction) &lua_wrapper_drawSprite);
  lua.Lua_register("drawSpriteRegion", (const lua_CFunction) &lua_wrapper_drawSpriteRegion);
  lua.Lua_register("drawAnimSprite", (const lua_CFunction) &lua_wrapper_drawAnimSprite);
  lua.Lua_register("log", (const lua_CFunction) &lua_wrapper_log);
  lua.Lua_register("drawString", (const lua_CFunction) &lua_wrapper_drawString);
  lua.Lua_register("drawRect", (const lua_CFunction) &lua_wrapper_drawRect);
  lua.Lua_register("fillRect", (const lua_CFunction) &lua_wrapper_fillRect);
  lua.Lua_register("drawCircle", (const lua_CFunction) &lua_wrapper_drawCircle);
  lua.Lua_register("fillCircle", (const lua_CFunction) &lua_wrapper_fillCircle);
  lua.Lua_register("drawLine", (const lua_CFunction) &lua_wrapper_drawLine);
  lua.Lua_register("drawFastHLine", (const lua_CFunction) &lua_wrapper_drawFastHLine);
  lua.Lua_register("drawFastWLine", (const lua_CFunction) &lua_wrapper_drawFastWLine);
  lua.Lua_register("fillScreen", (const lua_CFunction) &lua_wrapper_fillScreen);
  lua.Lua_register("setTextColor", (const lua_CFunction) &lua_wrapper_setTextColor);
  lua.Lua_register("setTextSize", (const lua_CFunction) &lua_wrapper_setTextSize);
  lua.Lua_register("setTextDatum", (const lua_CFunction) &lua_wrapper_setTextDatum);
  lua.Lua_register("setCursor", (const lua_CFunction) &lua_wrapper_setCursor);
  lua.Lua_register("print", (const lua_CFunction) &lua_wrapper_print);
  lua.Lua_register("println", (const lua_CFunction) &lua_wrapper_println);
  lua.Lua_register("cls", (const lua_CFunction) &lua_wrapper_cls);
  lua.Lua_register("prefsSetString", (const lua_CFunction) &lua_wrapper_prefsSetString);
  lua.Lua_register("prefsGetString", (const lua_CFunction) &lua_wrapper_prefsGetString);
  lua.Lua_register("prefsSetInt", (const lua_CFunction) &lua_wrapper_prefsSetInt);
  lua.Lua_register("prefsGetInt", (const lua_CFunction) &lua_wrapper_prefsGetInt);
  lua.Lua_register("prefsSetNumber", (const lua_CFunction) &lua_wrapper_prefsSetNumber);
  lua.Lua_register("prefsGetNumber", (const lua_CFunction) &lua_wrapper_prefsGetNumber);
  lua.Lua_register("closeProgressionMenu", (const lua_CFunction) &lua_wrapper_closeProgressionMenu);
  bindingsInitiated = true;
}

void initGames_lua(String gamePath, GameConfig* gameConfig, String* errorMessage) {
  initLuaBindings();
  luaGamePath = gamePath;

  setGamePrefsNamespace(gameConfig->prefsNamespace.c_str());

  String initScript = readFileToString((gamePath + "init.lua").c_str());
  if (!initScript.isEmpty()) {
    Serial.println("Running init script");
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
                          "cumulativeError="+String(blowData->cumulativeError)+"\n"+\
                          "fails="+String(blowData->fails)+"\n"+\
                          "taskType="+String(blowData->taskType)+"\n"+\
                          "lastBlowStatus="+String(blowData->lastBlowStatus)+"\n"+\
                          "totalTimeSpentBreathing="+String(blowData->totalTimeSpentBreathing)+"\n"+\
                          "taskStartMs="+String(blowData->taskStartMs);
  lua.Lua_dostring(&blowDataString);
}

void updateJumpData(JumpData* jumpData) {
  String jumpDataString = "ms = "+jumpData->ms;
  lua.Lua_dostring(&jumpDataString);
}

void drawShortBlowGame_lua(DISPLAY_T* display, BlowData* blowData, String* errorMessage) {
  String shortBlowScript = readFileToString((luaGamePath + "shortBlow.lua").c_str());
  luaDisplay = display;
  updateBlowData(blowData);
  lua.Lua_dostring(&shortBlowScript);
}

void drawLongBlowGame_lua(DISPLAY_T* display, BlowData* blowData, String* errorMessage) {
  String longBlowScript = readFileToString((luaGamePath + "longBlow.lua").c_str());
  luaDisplay = display;
  updateBlowData(blowData);
  lua.Lua_dostring(&longBlowScript);
}

void drawEqualBlowGame_lua(DISPLAY_T* display, BlowData* blowData, String* errorMessage) {
  String equalBlowScript = readFileToString((luaGamePath + "equalBlow.lua").c_str());
  luaDisplay = display;
  updateBlowData(blowData);
  lua.Lua_dostring(&equalBlowScript);
}

void drawTrampolineGame_lua(DISPLAY_T* display, JumpData* jumpData, String* errorMessage) {
  String trampolineScript = readFileToString((luaGamePath + "trampoline.lua").c_str());
  luaDisplay = display;
  updateJumpData(jumpData);
  lua.Lua_dostring(&trampolineScript);
}

void drawInhalationGame_lua(DISPLAY_T* display, BlowData* blowData, String* errorMessage) {
  String inhalationScript = readFileToString((luaGamePath + "inhalationBlow.lua").c_str());
  luaDisplay = display;
  updateBlowData(blowData);
  lua.Lua_dostring(&inhalationScript);
}

bool displayProgressionMenu_lua(DISPLAY_T *display, String *errorMessage) {
  luaProgressionMenuRunning = true;
  String progressionMenuScript = readFileToString((luaGamePath + "progressionMenu.lua").c_str());
  if (progressionMenuScript.isEmpty()) {
    errorMessage->concat("Failed to load progression menu script");
    return false;
  }
  luaDisplay = display;
  lua.Lua_dostring(&progressionMenuScript);
  return luaProgressionMenuRunning;
}