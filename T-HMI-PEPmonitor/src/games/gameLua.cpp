#include "gameLua.h"
#include "hardware/sdHandler.h"
#include "hardware/gfxHandler.hpp"
#include "hardware/prefsHandler.h"

static String luaGamePath;
lua_State* luaState;

DISPLAY_T* luaDisplay;
bool luaProgressionMenuRunning;
bool luaStrictMode = false;
bool luaCacheGameCode = false;

#define SPRITE_COUNT_LIMIT 100

struct SpriteMetadata {
  uint32_t frameW;
  uint32_t frameH;
  int32_t maskingColor;
};

static SpriteMetadata spriteMetadata[50];

// Block of 50 sprites to be used within Lua games
static TFT_eSprite* sprites = nullptr;

String lua_dofile(const String path, bool dontCache) {
  static char* script[10];
  static String cachedPath[10];
  String result;
  int cacheSlot = -1;
  if (luaCacheGameCode && !dontCache) {
    //Serial.println("Caching enalbed");
    for (int i=0;i<10;i++) {
      if (cachedPath[i] == path) {
        cacheSlot = i;
        //Serial.println("Found cached script "+path+" in slot "+String(cacheSlot));
        break;
      }
    }
    if (cacheSlot == -1) {
      for (int i=0;i<10;i++) {
        if (cachedPath[i].isEmpty()) {
          cacheSlot = i;
          //Serial.println("Caching script "+path+" in slot "+String(cacheSlot));
          
          Serial.print("Collect garbage before caching script, free RAM before: ");
          Serial.println(ESP.getFreeHeap());
          lua_gc(luaState, LUA_GCCOLLECT, 0);
          Serial.print("Free RAM after: ");
          Serial.println(ESP.getFreeHeap());
          
          if (script[cacheSlot] != nullptr) {
            free(script[cacheSlot]);
          }
          script[cacheSlot] = readFileToNewPSBuffer(path.c_str());
          cachedPath[cacheSlot] = path;
          if (script[cacheSlot] == nullptr) {
            result = "Failed to load lua script "+path;
            return result;
          }
          break;
        }
      }
    }
    if (cacheSlot == -1) {
      //Serial.println("Cache full, removing oldest script "+cachedPath[0]);
      for (int i=0;i<9;i++) {
        if (script[i] != nullptr) {
          free(script[i]);
        }
        script[i] = script[i+1];
        cachedPath[i] = cachedPath[i+1];
      }
      cacheSlot = 9;
      script[cacheSlot] = readFileToNewPSBuffer(path.c_str());
      cachedPath[cacheSlot] = path;
      if (script[cacheSlot] == nullptr) {
        result = "Failed to load lua script "+path;
        return result;
      }
    }
  } else {
    //Serial.println("Caching disabled");
    script[0] = readFileToNewPSBuffer(path.c_str());
    cacheSlot = 0;
    if (script[cacheSlot] == nullptr) {
      result = "Failed to load lua script "+path;
      return result;
    }
  }
  if (luaL_dostring(luaState, script[cacheSlot])) {
    result = "# lua error:\n" + String(lua_tostring(luaState, -1));
    lua_pop(luaState, 1);
    Serial.println("Error in lua file "+path+": " + result);
    if (luaStrictMode) {
      checkFailWithMessage("Error in lua file "+path+": " + result);
    }
  }
  if (!luaCacheGameCode || dontCache) {
    if (script[cacheSlot] != nullptr) {
      free(script[cacheSlot]);
      script[cacheSlot] = nullptr;
    }
  }
  return result;
}

String lua_dofile(const String path) {
  return lua_dofile(path, false);
}

String lua_dostring(const char* script) {
  String result;
  if (luaL_dostring(luaState, script)) {
    result += "# lua error:\n" + String(lua_tostring(luaState, -1));
    lua_pop(luaState, 1);
    Serial.println("Error in lua string: " + result);
    if (luaStrictMode) {
      checkFailWithMessage("Error in lua string: " + result);
    }
  }
  return result;
}

static int lua_wrapper_disableCaching(lua_State* luaState) {
  luaCacheGameCode = false;
  return 0;
}

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
  String error = lua_dostring(script.c_str());
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
  int32_t maskingColor = luaL_optinteger(luaState, 3, -1);

  Serial.print("Collect garbage before loading sprite, free RAM before: ");
  Serial.println(ESP.getFreeHeap());
  lua_gc(luaState, LUA_GCCOLLECT, 0);
  Serial.print("Free RAM after: ");
  Serial.println(ESP.getFreeHeap());

  for (int32_t i=0;i<SPRITE_COUNT_LIMIT;i++) {
    if (!sprites[i].created()) {
      if (!loadBmp(&sprites[i], path, flipped, maskingColor)) {
        Serial.println("Failed to load sprite "+path);
        if (luaStrictMode) {
          checkFailWithMessage("Failed to load sprite "+path);
        }
        lua_pushinteger(luaState, -1);
        return 1;
      }
      spriteMetadata[i].frameW = sprites[i].width();
      spriteMetadata[i].frameH = sprites[i].height();
      spriteMetadata[i].maskingColor = maskingColor;
      Serial.print("Found sprite slot: ");
      Serial.println(i);
      lua_pushinteger(luaState, i);
      return 1;
    }
  }
  Serial.println("Failed to find sprite slot for sprite "+path);
  if (luaStrictMode) {
    checkFailWithMessage("Failed to find sprite slot for sprite "+path);
  }
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
  int32_t maskingColor = luaL_optinteger(luaState, 5, -1);

  Serial.print("Collect garbage before loading sprite, free RAM before: ");
  Serial.println(ESP.getFreeHeap());
  lua_gc(luaState, LUA_GCCOLLECT, 0);
  Serial.print("Free RAM after: ");
  Serial.println(ESP.getFreeHeap());

  for (int32_t i=0;i<SPRITE_COUNT_LIMIT;i++) {
    if (!sprites[i].created()) {
      if (maskingColor != -1) {
        loadBmp(&sprites[i], path, flipped, maskingColor);
      } else {
        loadBmp(&sprites[i], path, flipped);
      }
      Serial.print("Found sprite slot: ");
      Serial.println(i);
      spriteMetadata[i].frameW = luaL_checknumber(luaState, 2);
      spriteMetadata[i].frameH = luaL_checknumber(luaState, 3);
      spriteMetadata[i].maskingColor = maskingColor;
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
  if ((handle<0) || (handle>=SPRITE_COUNT_LIMIT) || (!sprites[handle].created())) {
    Serial.println("ERROR: Could not draw sprite: invalid sprite handle!");
    if (luaStrictMode) {
      checkFailWithMessage("ERROR: Could not draw sprite: invalid sprite handle!");
    }
    return 0;
  }
  int32_t maskingColor = spriteMetadata[handle].maskingColor;
  if (maskingColor != -1) {
    sprites[handle].pushToSprite(luaDisplay, x, y, spriteMetadata[handle].maskingColor);
  } else {
    sprites[handle].pushToSprite(luaDisplay, x, y);
  }
  
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
  int32_t maskingColor = spriteMetadata[handle].maskingColor;
  if (maskingColor != -1) {
    sprites[handle].pushToSprite(luaDisplay, tx, ty, sx, sy, sw, sh, maskingColor);
  } else {
    sprites[handle].pushToSprite(luaDisplay, tx, ty, sx, sy, sw, sh);
  }
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
  
  int16_t sw = spriteMetadata[handle].frameW;
  int16_t sh = spriteMetadata[handle].frameH;

  int16_t cols = sprites[handle].width() / sw;

  int16_t col = frame % cols;
  int16_t row = frame / cols;

  int32_t maskingColor = spriteMetadata[handle].maskingColor;
  if (maskingColor != -1) {
    sprites[handle].pushToSprite(luaDisplay, tx, ty, sw*col, sh*row, sw, sh, maskingColor);
  } else {
    sprites[handle].pushToSprite(luaDisplay, tx, ty, sw*col, sh*row, sw, sh);
  }
  
  Serial.println("Draw anim sprite done");
  return 0;
}

static int lua_wrapper_drawSpriteToSprite(lua_State* luaState) {
  //Serial.print("Draw sprite ");
  int16_t srcHandle = luaL_checkinteger(luaState, 1);
  int16_t dstHandle = luaL_checkinteger(luaState, 2);
  //Serial.println(handle);
  int16_t x = luaL_checknumber(luaState, 3);
  int16_t y = luaL_checknumber(luaState, 4);

  if ((srcHandle<0) || (srcHandle>=SPRITE_COUNT_LIMIT) || (!sprites[srcHandle].created())) {
    Serial.println("ERROR: Could not draw sprite to sprite: invalid src sprite handle!");
    if (luaStrictMode) {
      checkFailWithMessage("ERROR: Could not draw sprite to sprite: invalid src sprite handle!");
    }
    return 0;
  }
  if ((dstHandle<0) || (dstHandle>=SPRITE_COUNT_LIMIT) || (!sprites[dstHandle].created())) {
    Serial.println("ERROR: Could not draw sprite to sprite: invalid dst sprite handle!");
    if (luaStrictMode) {
      checkFailWithMessage("ERROR: Could not draw sprite to sprite: invalid dst sprite handle!");
    }
    return 0;
  }

  int32_t maskingColor = spriteMetadata[srcHandle].maskingColor;
  Serial.print("PSTS: Masking color: ");
  Serial.println(maskingColor);
  if (maskingColor != -1) {
    sprites[srcHandle].pushToSprite(&sprites[dstHandle], x, y, maskingColor);
  } else {
    sprites[srcHandle].pushToSprite(&sprites[dstHandle], x, y);
  }
  //Serial.println("Draw sprite done");
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

void initLua() {
  static bool bindingsInitiated = false;
  if (bindingsInitiated) {
    return;
  }
  luaState = luaL_newstate();
  luaopen_base(luaState);
  luaopen_table(luaState);
  luaopen_string(luaState);
  luaopen_math(luaState);

  sprites = (TFT_eSprite*) heap_caps_malloc(sizeof(TFT_eSprite) * SPRITE_COUNT_LIMIT, MALLOC_CAP_SPIRAM);
  if (!sprites) {
    Serial.println("Failed to allocate sprites in PSRAM!");
    checkFailWithMessage("init: Failed to allocate sprites in PSRAM!");
    return;
  }

  for (size_t i = 0; i < SPRITE_COUNT_LIMIT; ++i) {
    new (&sprites[i]) TFT_eSprite(&tft);
  }


  lua_register(luaState, "serialPrintln", (lua_CFunction) &lua_wrapper_serialPrintln);
  lua_register(luaState, "serialPrint", (lua_CFunction) &lua_wrapper_serialPrintln);
  lua_register(luaState, "runScript", (lua_CFunction) &lua_wrapper_runScript);
  lua_register(luaState, "loadSprite", (lua_CFunction) &lua_wrapper_loadSprite);
  lua_register(luaState, "loadAnimSprite", (lua_CFunction) &lua_wrapper_loadAnimSprite);
  lua_register(luaState, "freeSprite", (lua_CFunction) &lua_wrapper_freeSprite);
  lua_register(luaState, "drawSprite", (lua_CFunction) &lua_wrapper_drawSprite);
  lua_register(luaState, "drawSpriteRegion", (lua_CFunction) &lua_wrapper_drawSpriteRegion);
  lua_register(luaState, "drawAnimSprite", (lua_CFunction) &lua_wrapper_drawAnimSprite);
  lua_register(luaState, "drawSpriteToSprite", (lua_CFunction) &lua_wrapper_drawSpriteToSprite);
  lua_register(luaState, "spriteWidth", (lua_CFunction) &lua_wrapper_spriteWidth);
  lua_register(luaState, "spriteHeight", (lua_CFunction) &lua_wrapper_spriteHeight);
  lua_register(luaState, "log", (lua_CFunction) &lua_wrapper_log);
  lua_register(luaState, "drawString", (lua_CFunction) &lua_wrapper_drawString);
  lua_register(luaState, "drawRect", (lua_CFunction) &lua_wrapper_drawRect);
  lua_register(luaState, "fillRect", (lua_CFunction) &lua_wrapper_fillRect);
  lua_register(luaState, "drawCircle", (lua_CFunction) &lua_wrapper_drawCircle);
  lua_register(luaState, "fillCircle", (lua_CFunction) &lua_wrapper_fillCircle);
  lua_register(luaState, "drawLine", (lua_CFunction) &lua_wrapper_drawLine);
  lua_register(luaState, "drawFastHLine", (lua_CFunction) &lua_wrapper_drawFastHLine);
  lua_register(luaState, "drawFastWLine", (lua_CFunction) &lua_wrapper_drawFastWLine);
  lua_register(luaState, "fillScreen", (lua_CFunction) &lua_wrapper_fillScreen);
  lua_register(luaState, "setTextColor", (lua_CFunction) &lua_wrapper_setTextColor);
  lua_register(luaState, "setTextSize", (lua_CFunction) &lua_wrapper_setTextSize);
  lua_register(luaState, "setTextDatum", (lua_CFunction) &lua_wrapper_setTextDatum);
  lua_register(luaState, "setCursor", (lua_CFunction) &lua_wrapper_setCursor);
  lua_register(luaState, "print", (lua_CFunction) &lua_wrapper_print);
  lua_register(luaState, "println", (lua_CFunction) &lua_wrapper_println);
  lua_register(luaState, "cls", (lua_CFunction) &lua_wrapper_cls);
  lua_register(luaState, "prefsSetString", (lua_CFunction) &lua_wrapper_prefsSetString);
  lua_register(luaState, "prefsGetString", (lua_CFunction) &lua_wrapper_prefsGetString);
  lua_register(luaState, "prefsSetInt", (lua_CFunction) &lua_wrapper_prefsSetInt);
  lua_register(luaState, "prefsGetInt", (lua_CFunction) &lua_wrapper_prefsGetInt);
  lua_register(luaState, "prefsSetNumber", (lua_CFunction) &lua_wrapper_prefsSetNumber);
  lua_register(luaState, "prefsGetNumber", (lua_CFunction) &lua_wrapper_prefsGetNumber);
  lua_register(luaState, "closeProgressionMenu", (lua_CFunction) &lua_wrapper_closeProgressionMenu);
  lua_register(luaState, "constrain", (lua_CFunction) &lua_wrapper_constrain);
  lua_register(luaState, "isTouchInZone", (lua_CFunction) &lua_wrapper_isTouchInZone);
  lua_register(luaState, "getFreeRAM", (lua_CFunction) &lua_wrapper_getFreeRAM);
  lua_register(luaState, "disableCaching", (lua_CFunction) &lua_wrapper_disableCaching);
  bindingsInitiated = true;
}

void initGames_lua(String gamePath, GameConfig* gameConfig, String* errorMessage) {
  String ignoreErrorMessage;
  initLua();
  String msString = "ms="+String(millis());
  lua_dostring(msString.c_str());
  luaGamePath = gamePath;
  String luaGameIniPath = gamePath + "gameconfig.ini";
  luaStrictMode = getIniValue(luaGameIniPath, "[game]", "strictMode", &ignoreErrorMessage).equalsIgnoreCase("true");
  luaCacheGameCode = !getIniValue(luaGameIniPath, "[game]", "caceGameCode", &ignoreErrorMessage).equalsIgnoreCase("false");

  setGamePrefsNamespace(gameConfig->prefsNamespace.c_str());

  lua_dofile(luaGamePath + "init.lua");
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
  lua_dostring(blowDataString.c_str());
  lastMs = blowData->ms;
}

void updateJumpData(JumpData* jumpData) {
  String jumpDataString = "ms="+jumpData->ms;
  lua_dostring(jumpDataString.c_str());
}

void drawShortBlowGame_lua(DISPLAY_T* display, BlowData* blowData, String* errorMessage) {
  luaDisplay = display;
  updateBlowData(blowData);
  lua_dofile(luaGamePath + "shortBlow.lua");
}

void drawLongBlowGame_lua(DISPLAY_T* display, BlowData* blowData, String* errorMessage) {
  luaDisplay = display;
  updateBlowData(blowData);
  lua_dofile(luaGamePath + "longBlow.lua");
}

void drawEqualBlowGame_lua(DISPLAY_T* display, BlowData* blowData, String* errorMessage) {
  luaDisplay = display;
  updateBlowData(blowData);
  lua_dofile(luaGamePath + "equalBlow.lua");
}

void drawTrampolineGame_lua(DISPLAY_T* display, JumpData* jumpData, String* errorMessage) {
  luaDisplay = display;
  updateJumpData(jumpData);
  lua_dofile(luaGamePath + "trampoline.lua");
}

void drawInhalationGame_lua(DISPLAY_T* display, BlowData* blowData, String* errorMessage) {
  luaDisplay = display;
  updateBlowData(blowData);
  lua_dofile(luaGamePath + "inhalationBlow.lua");
}

bool displayProgressionMenu_lua(DISPLAY_T *display, String *errorMessage) {
  luaProgressionMenuRunning = true;
  luaDisplay = display;
  lua_dostring(("ms="+String(millis())).c_str());
  String error = lua_dofile(luaGamePath + "progressionMenu.lua");
  if (!error.isEmpty()) {
    errorMessage->concat(error);
    return false;
  }
  return luaProgressionMenuRunning;
}

void endGame_lua(String *errorMessage) {
  lua_dofile(luaGamePath + "end.lua", true);
  Serial.print("Collect garbage after running end script, free RAM before: ");
  Serial.println(ESP.getFreeHeap());
  lua_gc(luaState, LUA_GCCOLLECT, 0);
  Serial.print("Free RAM after: ");
  Serial.println(ESP.getFreeHeap());
}