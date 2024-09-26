#ifndef __MONSTER_DETAILS_H__
#define __MONSTER_DETAILS_H__

#include <Arduino.h>

#define TOTAL_MONSTER_NUMBER 151
#define MAX_MONSTER_NUMBER 400 // The maximum number of monsters that the save game will ever be compatible. Should not be changed.

#define ATTACK_ID_EMBER 1
#define ATTACK_ID_SEED 2
#define ATTACK_ID_WATER 3
#define ATTACK_ID_STRINGSHOT 4
#define ATTACK_ID_CONFUSION 5
#define ATTACK_ID_BALL 6
#define ATTACK_ID_RARE_CANDY 7
#define ATTACK_ID_POISONSTING 8
#define ATTACK_ID_AIR_SLASH 9
#define ATTACK_ID_SCRATCH 10
#define ATTACK_ID_LIGHTNING 11
#define ATTACK_ID_LEER 12
#define ATTACK_ID_SING 13
#define ATTACK_ID_ROCK_THROW 14
#define ATTACK_ID_DRAGON_RAGE 15
#define ATTACK_ID_KARATE_CHOP 16
#define ATTACK_ID_ICE_BEAM 17
#define ATTACK_ID_SAND_TOMB 18
#define ATTACK_ID_SHADOW_BALL 19
#define ATTACK_ID_METAL_CLAW 20
#define ATTACK_ID_BITE 21

#define BASIC_MONSTER 0x8000
#define SAFARI_MONSTER_RARITY_1 0x2000 // Common
#define SAFARI_MONSTER_RARITY_2 0x4000
#define SAFARI_MONSTER_RARITY_3 0x6000 // Rare

uint16_t getMonsterEvolvesTo(uint16_t monsterId);
bool isBasicMonster(uint16_t monsterId);
uint16_t getSafariMonster(uint8_t rarity); //rarity can between 1-4

extern const String monsterName[387];
extern const String monsterImagePath[387];
extern const uint16_t monsterEvolvesTo[387];
extern const uint8_t monsterAttackId[387];

#endif /*__MONSTER_DETAILS_H__*/