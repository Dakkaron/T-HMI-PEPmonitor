#include "gameLua.h"
#include "hardware/sdHandler.h"
#include "hardware/gfxHandler.hpp"
#include "hardware/prefsHandler.h"

LuaWrapper lua;
static String luaGamePath;

DISPLAY_T* luaDisplay;
bool luaProgressionMenuRunning;
bool luaStrictMode = false;
bool luaCacheGameCode = false;

#define SPRITE_COUNT_LIMIT 50

struct SpriteMetadata {
  uint32_t frameW;
  uint32_t frameH;
};

static SpriteMetadata spriteMetadata[50];

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

static int lua_wrapper_serialPrint(lua_State* luaState) {
  Serial.println(luaL_checkstring(luaState, 1));
  return 0;
}


static int lua_wrapper_serialPrintln(lua_State* luaState) {
  Serial.println(luaL_checkstring(luaState, 1));
  return 0;
}

static int lua_wrapper_runScript(lua_State* luaState) {
  String path = luaGamePath + luaL_checkstring(luaState, 1);
  String script = readFileToString((path).c_str());
  if (script.isEmpty()) {
    Serial.println("Error: Failed to load script "+path);
    return 0;
  }
  String error = lua.Lua_dostring(&script);
  if (!error.isEmpty()) {
    Serial.println("Error in script " + path + ": " + error);
  }
  return 0;
}

static int lua_wrapper_loadSprite(lua_State* luaState) {
  Serial.print("Load BMP sprite ");
  String path = luaGamePath + luaL_checkstring(luaState, 1);
  Serial.println(path);
  int32_t flipped = luaL_optinteger(luaState, 2, 0);
  for (int32_t i=0;i<SPRITE_COUNT_LIMIT;i++) {
    if (!sprites[i].created()) {
      if (!loadBmp(&sprites[i], path, flipped)) {
        Serial.println("Failed to load sprite.");
        lua_pushinteger(luaState, -1);
        return 1;
      }
      spriteMetadata[i].frameW = sprites[i].width();
      spriteMetadata[i].frameH = sprites[i].height();
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

static int lua_wrapper_freeSprite(lua_State* luaState) {
  Serial.print("Free BMP sprite ");
  int16_t handle = luaL_checkinteger(luaState, 1);
  Serial.println(handle);
  if (sprites[handle].created()) {
    sprites[handle].deleteSprite();
  }
  return 0;
}

static int lua_wrapper_loadAnimSprite(lua_State* luaState) {
  Serial.print("Load animated BMP sprite ");
  String path = luaGamePath + luaL_checkstring(luaState, 1);
  Serial.println(path);
  int32_t flipped = luaL_optinteger(luaState, 4, 0);
  for (int32_t i=0;i<SPRITE_COUNT_LIMIT;i++) {
    if (!sprites[i].created()) {
      loadBmp(&sprites[i], path, flipped);
      Serial.print("Found sprite slot: ");
      Serial.println(i);
      spriteMetadata[i].frameW = luaL_checknumber(luaState, 2);
      spriteMetadata[i].frameH = luaL_checknumber(luaState, 3);
      lua_pushinteger(luaState, i);
      return 1;
    }
  }
  Serial.println("Failed to find sprite slot");
  lua_pushinteger(luaState, -1);
  return 1;
}

static int lua_wrapper_drawSprite(lua_State* luaState) {
  //Serial.print("Draw sprite ");
  int16_t handle = luaL_checkinteger(luaState, 1);
  //Serial.println(handle);
  int16_t x = luaL_checknumber(luaState, 2);
  int16_t y = luaL_checknumber(luaState, 3);
  int16_t transp = luaL_optinteger(luaState, 4, 0x0000);
  if (handle<0 || handle>=SPRITE_COUNT_LIMIT) {
    Serial.println("Could not draw sprite: invalid sprite handle");
    return 0;
  }
  sprites[handle].pushToSprite(luaDisplay, x, y, transp);
  //Serial.println("Draw sprite done");
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
  Serial.print("Draw anim sprite");
  int16_t handle = luaL_checkinteger(luaState, 1);
  Serial.println(handle);
  int16_t tx = luaL_checknumber(luaState, 2);
  int16_t ty = luaL_checknumber(luaState, 3);
  int16_t frame = luaL_checknumber(luaState, 4);
  int16_t transp = luaL_optnumber(luaState, 5, 0x0000);
  
  int16_t sw = spriteMetadata[handle].frameW;
  int16_t sh = spriteMetadata[handle].frameH;

  int16_t cols = sprites[handle].width() / sw;
  int16_t rows = sprites[handle].height() / sh;

  int16_t col = frame % cols;
  int16_t row = frame / cols;

  sprites[handle].pushToSprite(luaDisplay, tx, ty, sw*col, sh*row, sw, sh, transp);
  Serial.println("Draw anim sprite done");
  return 0;
}

static int lua_wrapper_spriteHeight(lua_State* luaState) {
  int16_t handle = luaL_checkinteger(luaState, 1);
  lua_pushinteger(luaState, spriteMetadata[handle].frameH);
  return 1;
}

static int lua_wrapper_spriteWidth(lua_State* luaState) {
  int16_t handle = luaL_checkinteger(luaState, 1);
  lua_pushinteger(luaState, spriteMetadata[handle].frameW);
  return 1;
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
  int value = luaL_checknumber(luaState, 2);
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

static int lua_wrapper_constrain(lua_State* luaState) {
  float val = luaL_checknumber(luaState, 1);
  float min = luaL_checknumber(luaState, 2);
  float max = luaL_checknumber(luaState, 3);
  lua_pushnumber(luaState, constrain(val, min, max));
  return 1;
}

static int lua_wrapper_closeProgressionMenu(lua_State* luaState) {
  luaProgressionMenuRunning = false;
  return 0;
}

static int lua_wrapper_isTouchInZone(lua_State* luaState) {
  int16_t x = luaL_checknumber(luaState, 1);
  int16_t y = luaL_checknumber(luaState, 2);
  int16_t w = luaL_checknumber(luaState, 3);
  int16_t h = luaL_checknumber(luaState, 4);
  bool result = isTouchInZone(x, y, w, h);
  lua_pushboolean(luaState, result);
  return 1;
}

static int lua_wrapper_getFreeRAM(lua_State* luaState) {
  lua_pushinteger(luaState, ESP.getFreeHeap());
  return 1;
}

void initLuaBindings() {
  static bool bindingsInitiated = false;
  if (bindingsInitiated) {
    return;
  }
  lua.Lua_register("serialPrintln", (const lua_CFunction) &lua_wrapper_serialPrintln);
  lua.Lua_register("serialPrint", (const lua_CFunction) &lua_wrapper_serialPrintln);
  lua.Lua_register("runScript", (const lua_CFunction) &lua_wrapper_runScript);
  lua.Lua_register("loadSprite", (const lua_CFunction) &lua_wrapper_loadSprite);
  lua.Lua_register("loadAnimSprite", (const lua_CFunction) &lua_wrapper_loadAnimSprite);
  lua.Lua_register("freeSprite", (const lua_CFunction) &lua_wrapper_freeSprite);
  lua.Lua_register("drawSprite", (const lua_CFunction) &lua_wrapper_drawSprite);
  lua.Lua_register("drawSpriteRegion", (const lua_CFunction) &lua_wrapper_drawSpriteRegion);
  lua.Lua_register("drawAnimSprite", (const lua_CFunction) &lua_wrapper_drawAnimSprite);
  lua.Lua_register("spriteWidth", (const lua_CFunction) &lua_wrapper_spriteWidth);
  lua.Lua_register("spriteHeight", (const lua_CFunction) &lua_wrapper_spriteHeight);
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
  lua.Lua_register("constrain", (const lua_CFunction) &lua_wrapper_constrain);
  lua.Lua_register("isTouchInZone", (const lua_CFunction) &lua_wrapper_isTouchInZone);
  lua.Lua_register("getFreeRAM", (const lua_CFunction) &lua_wrapper_getFreeRAM);
  bindingsInitiated = true;
}

void initGames_lua(String gamePath, GameConfig* gameConfig, String* errorMessage) {
  String ignoreErrorMessage;
  initLuaBindings();
  String msString = "ms="+String(millis());
  lua.Lua_dostring(&msString);
  luaGamePath = gamePath;
  String luaGameIniPath = gamePath + "gameconfig.ini";
  luaStrictMode = getIniValue(luaGameIniPath, "[game]", "strictMode", &ignoreErrorMessage).equalsIgnoreCase("true");
  luaCacheGameCode = !getIniValue(luaGameIniPath, "[game]", "caceGameCode", &ignoreErrorMessage).equalsIgnoreCase("false");

  setGamePrefsNamespace(gameConfig->prefsNamespace.c_str());

  String initScript = readFileToString((gamePath + "init.lua").c_str());
  if (!initScript.isEmpty()) {
    Serial.println("Running init script");
    String error = lua.Lua_dostring(&initScript);
    if (!error.isEmpty()) {
      Serial.println("Error in init.lua: " + error);
      if (luaStrictMode) {
        checkFailWithMessage("Error in init.lua: " + error);
      }
    }
  }
}

void updateBlowData(BlowData* blowData) {
  static uint32_t lastMs = 0;
  static int32_t lastKnownTaskNumber = -1;
  int32_t taskNumber = blowData->taskNumber + blowData->cycleNumber * blowData->totalTaskNumber;
  String blowDataString = "currentlyBlowing="+String(blowData->currentlyBlowing ? "true" : "false")+"\n"+\
                          "ms="+String(blowData->ms)+"\n"+\
                          "msDelta="+String(blowData->ms - lastMs)+"\n"+\
                          "blowStartMs="+String(blowData->blowStartMs)+"\n"+\
                          "blowEndMs="+String(blowData->blowEndMs)+"\n"+\
                          "targetDurationMs="+String(blowData->targetDurationMs)+"\n"+\
                          "cycleNumber="+String(blowData->cycleNumber)+"\n"+\
                          "totalCycleNumber="+String(blowData->totalCycleNumber)+"\n"+\
                          "blowNumber="+String(blowData->blowCount)+"\n"+\
                          "totalBlowNumber="+String(blowData->totalBlowCount)+"\n"+\
                          "pressure="+String(blowData->pressure)+"\n"+\
                          "peakPressure="+String(blowData->peakPressure)+"\n"+\
                          "minPressure="+String(blowData->minPressure)+"\n"+\
                          "cumulativeError="+String(blowData->cumulativeError)+"\n"+\
                          "fails="+String(blowData->fails)+"\n"+\
                          "taskType="+String(blowData->taskType)+"\n"+\
                          "lastBlowStatus="+String(blowData->lastBlowStatus)+"\n"+\
                          "totalTimeSpentBreathing="+String(blowData->totalTimeSpentBreathing)+"\n"+\
                          "taskStartMs="+String(blowData->taskStartMs)+"\n"+\
                          "cumulatedTaskNumber="+String(blowData->taskNumber + blowData->cycleNumber * blowData->totalTaskNumber)+"\n"+\
                          "taskNumber="+String(taskNumber)+"\n"+\
                          "totalTaskNumber="+String(blowData->totalTaskNumber)+"\n"+
                          "isNewTask="+String(taskNumber != lastKnownTaskNumber ? "true" : "false");
  lastKnownTaskNumber = taskNumber;
  lua.Lua_dostring(&blowDataString);
  lastMs = blowData->ms;
}

void updateJumpData(JumpData* jumpData) {
  String jumpDataString = "ms="+jumpData->ms;
  lua.Lua_dostring(&jumpDataString);
}

void drawShortBlowGame_lua(DISPLAY_T* display, BlowData* blowData, String* errorMessage) {
  String script = readFileToString((luaGamePath + "shortBlow.lua").c_str());
  luaDisplay = display;
  updateBlowData(blowData);
  String error = lua.Lua_dostring(&script);
  if (!error.isEmpty()) {
    Serial.println("Error in shortBlow.lua: " + error);
    if (luaStrictMode) {
      checkFailWithMessage("Error in shortBlow.lua: " + error);
    }
  }
}

void drawLongBlowGame_lua(DISPLAY_T* display, BlowData* blowData, String* errorMessage) {
  String script = readFileToString((luaGamePath + "longBlow.lua").c_str());
  luaDisplay = display;
  updateBlowData(blowData);
  String error = lua.Lua_dostring(&script);
  if (!error.isEmpty()) {
    Serial.println("Error in longBlow.lua: " + error);
    if (luaStrictMode) {
      checkFailWithMessage("Error in longBlow.lua: " + error);
    }
  }
}

void drawEqualBlowGame_lua(DISPLAY_T* display, BlowData* blowData, String* errorMessage) {
  String script = readFileToString((luaGamePath + "equalBlow.lua").c_str());
  luaDisplay = display;
  updateBlowData(blowData);
  String error = lua.Lua_dostring(&script);
  if (!error.isEmpty()) {
    Serial.println("Error in equalBlow.lua: " + error);
    if (luaStrictMode) {
      checkFailWithMessage("Error in equalBlow.lua: " + error);
    }
  }
}

void drawTrampolineGame_lua(DISPLAY_T* display, JumpData* jumpData, String* errorMessage) {
  String script = readFileToString((luaGamePath + "trampoline.lua").c_str());
  luaDisplay = display;
  updateJumpData(jumpData);
  String error = lua.Lua_dostring(&script);
  if (!error.isEmpty()) {
    Serial.println("Error in trampoline.lua: " + error);
    if (luaStrictMode) {
      checkFailWithMessage("Error in trampoline.lua: " + error);
    }
  }
}

void drawInhalationGame_lua(DISPLAY_T* display, BlowData* blowData, String* errorMessage) {
  String script = readFileToString((luaGamePath + "inhalationBlow.lua").c_str());
  luaDisplay = display;
  updateBlowData(blowData);
  String error = lua.Lua_dostring(&script);
  if (!error.isEmpty()) {
    Serial.println("Error in inhalationBlow.lua: " + error);
    if (luaStrictMode) {
      checkFailWithMessage("Error in inhalationBlow.lua: " + error);
    }
  }
}

bool displayProgressionMenu_lua(DISPLAY_T *display, String *errorMessage) {
  luaProgressionMenuRunning = true;
  String script = readFileToString((luaGamePath + "progressionMenu.lua").c_str());
  if (script.isEmpty()) {
    errorMessage->concat("Failed to load progression menu script");
    return false;
  }
  luaDisplay = display;
  String error = lua.Lua_dostring(&script);
  if (!error.isEmpty()) {
    Serial.println("Error in progressionMenu.lua: " + error);
    if (luaStrictMode) {
      checkFailWithMessage("Error in progressionMenu.lua: " + error);
    }
  }
  return luaProgressionMenuRunning;
}

void endGame_lua(String *errorMessage) {
  String script = readFileToString((luaGamePath + "end.lua").c_str());
  String error = lua.Lua_dostring(&script);
  if (!error.isEmpty()) {
    Serial.println("Error in end.lua: " + error);
    if (luaStrictMode) {
      checkFailWithMessage("Error in end.lua: " + error);
    }
  }
}