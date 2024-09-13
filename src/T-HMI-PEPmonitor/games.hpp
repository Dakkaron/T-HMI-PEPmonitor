#ifndef __GAMES_H__
#define __GAMES_H__
#include <Preferences.h>

#include <Arduino.h>

#include "config.h"
#include "constants.h"
#include "EEPROM.h"
#include "gfxHandler.hpp"
#include "sdHandler.h"
#include "touchHandler.h"
#include "monsterDetails.h"

#define NUM_SHORT_BLOW_GAMES 2
#define NUM_LONG_BLOW_GAMES 3

#define GAME_DRAW_Y 18
#define GAME_SNOWMAN_DRAW_Y 1
#define SNOWFLAKE_MOVEMENT_INTERVAL 100 //given in ms

#define DRAW_PLAYER_ALIVE     0b00000001
#define DRAW_PLAYER_DEAD      0b00000010
#define DRAW_PLAYER_DODGE     0b00000100
#define DRAW_ENEMY_ALIVE      0b00001000
#define DRAW_ENEMY_DEAD       0b00010000
#define DRAW_ENEMY_DODGE      0b00100000

#define BITMAP_HLEFT_ALIGN   0b00000100
#define BITMAP_HCENTER       0b00000010
#define BITMAP_HRIGHT_ALIGN  0b00000001
#define BITMAP_VTOP_ALIGN    0b00001000
#define BITMAP_VCENTER       0b00010000
#define BITMAP_VBOTTOM_ALIGN 0b00100000

void initGames();
bool drawShortBlowGame(uint8_t index, DISPLAY_T* display, BlowData* blowData);
bool drawLongBlowGame(uint8_t index, DISPLAY_T* display, BlowData* blowData);

#define TRAINER_ANIM_PATH "/gfx/trainer/brendan.bmp"

#define ATTACK_SPRITE_NUMBER 16

typedef uint8_t (*AttackFunctionType)(DISPLAY_T* display, BlowData* blowData, bool playerIsAttacking, int32_t animTime, bool draw);

struct AttackData {
  uint8_t animFrames;
  String gfxPath;
  AttackFunctionType attackFunction;
};

#endif /* __GAMES_H__*/
