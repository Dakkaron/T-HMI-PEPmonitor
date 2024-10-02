#ifndef __GAMES_H__
#define __GAMES_H__
#include <Arduino.h>

#include "config.h"
#include "constants.h"
#include "EEPROM.h"
#include "gfxHandler.hpp"
#include "sdHandler.h"
#include "touchHandler.h"
#include "monsterDetails.h"
#include "prefsHandler.h"

#define NUM_SHORT_BLOW_GAMES 1
#define NUM_LONG_BLOW_GAMES 1
#define NUM_TRAMPOLINE_GAMES 1

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
void drawShortBlowGame(uint8_t index, DISPLAY_T* display, BlowData* blowData);
void drawLongBlowGame(uint8_t index, DISPLAY_T* display, BlowData* blowData);
void drawTrampolineGame(uint8_t index, DISPLAY_T* display, JumpData* jumpData);

#define TRAINER_ANIM_PATH "/gfx/trainer/brendan.bmp"

#endif /* __GAMES_H__*/
