#ifndef __MONSTER_DETAILS_H__
#define __MONSTER_DETAILS_H__

#include <Arduino.h>
#include "hardware/sdHandler.h"
#include "hardware/gfxHandler.hpp"

#define ATTACK_SPRITE_NUMBER 16

#define ATTACK_IDENTIFIER_RARE_CANDY "rare_candy"
#define ATTACK_IDENTIFIER_CATCH_BALL "catch_ball"

uint8_t attackFunction_confusion(DISPLAY_T* display, BlowData* blowData, bool playerIsAttacking, int32_t animTime, bool draw);
uint8_t attackFunction_ember(DISPLAY_T* display, BlowData* blowData, bool playerIsAttacking, int32_t animTime, bool draw);
uint8_t attackFunction_throwFastAnim(DISPLAY_T* display, BlowData* blowData, bool playerIsAttacking, int32_t animTime, bool draw);
uint8_t attackFunction_scratch(DISPLAY_T* display, BlowData* blowData, bool playerIsAttacking, int32_t animTime, bool draw);
uint8_t attackFunction_lightning(DISPLAY_T* display, BlowData* blowData, bool playerIsAttacking, int32_t animTime, bool draw);
uint8_t attackFunction_catch(DISPLAY_T* display, BlowData* blowData, bool playerIsAttacking, int32_t animTime, bool draw);
uint8_t attackFunction_rareCandy(DISPLAY_T* display, BlowData* blowData, bool playerIsAttacking, int32_t animTime, bool draw);

#define BASIC_MONSTER 0x8000
#define SAFARI_MONSTER_RARITY_1 0x2000 // Common
#define SAFARI_MONSTER_RARITY_2 0x4000
#define SAFARI_MONSTER_RARITY_3 0x6000 // Rare

uint16_t getMaxMonsterCount(String gameIniPath, String* errorMessage);
uint16_t getMonsterCount(String gameIniPath, String* errorMessage);
uint16_t getRandomMonsterId(String gameIniPath, String* errorMessage);
uint16_t getSafariMonster(String gameIniPath, uint8_t rarity, String* errorMessage); //rarity can between 1-4

typedef uint8_t (*AttackFunctionType)(DISPLAY_T* display, BlowData* blowData, bool playerIsAttacking, int32_t animTime, bool draw);

struct AttackData {
    String identifier = "";
    String imagePath = "";
    uint8_t animFrames = 0;
    AttackFunctionType attackFunction = NULL;
};

struct MonsterData {
    uint16_t id = 0;
    String name = "";
    String imagePath = "";
    AttackData attack;
    bool isBasicMonster = false;
    uint16_t evolvesTo = 0;
    uint8_t safariRarity = 0;
};

void loadAttackData(String gameIniPath, AttackData* attackData, String attackIdentifier, String* errorMessage);
void loadMonsterData(String gameIniPath, MonsterData* monsterData, uint16_t monsterId, String* errorMessage);

#endif /*__MONSTER_DETAILS_H__*/