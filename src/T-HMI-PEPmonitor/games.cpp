#include "games.hpp"
Preferences prefs;

uint8_t attackFunction_confusion(DISPLAY_T* display, BlowData* blowData, bool playerIsAttacking, int32_t animTime, bool draw);
uint8_t attackFunction_ember(DISPLAY_T* display, BlowData* blowData, bool playerIsAttacking, int32_t animTime, bool draw);
uint8_t attackFunction_throwFastAnim(DISPLAY_T* display, BlowData* blowData, bool playerIsAttacking, int32_t animTime, bool draw);
uint8_t attackFunction_scratch(DISPLAY_T* display, BlowData* blowData, bool playerIsAttacking, int32_t animTime, bool draw);
uint8_t attackFunction_lightning(DISPLAY_T* display, BlowData* blowData, bool playerIsAttacking, int32_t animTime, bool draw);
uint8_t attackFunction_catch(DISPLAY_T* display, BlowData* blowData, bool playerIsAttacking, int32_t animTime, bool draw);
uint8_t attackFunction_rareCandy(DISPLAY_T* display, BlowData* blowData, bool playerIsAttacking, int32_t animTime, bool draw);

const AttackData attackDataArray[] = {
  {0, "", NULL}, // no attack
  {8, "/gfx/attacks/fire.bmp", attackFunction_ember}, // ATTACK_ID_EMBER
  {3, "/gfx/attacks/seed.bmp", attackFunction_ember}, // ATTACK_ID_SEED
  {5, "/gfx/attacks/water_droplet_2.bmp", attackFunction_ember}, // ATTACK_ID_WATER
  {1, "/gfx/attacks/web.bmp", attackFunction_ember}, // ATTACK_ID_STRINGSHOT
  {0, "", attackFunction_confusion}, // ATTACK_ID_CONFUSION
  {3, "/gfx/attacks/ball.bmp", attackFunction_catch}, // ATTACK_ID_BALL
  {1, "/gfx/attacks/rare_candy.bmp", attackFunction_rareCandy}, // ATTACK_ID_RARE_CANDY
  {1, "/gfx/attacks/horn_hit.bmp", attackFunction_ember}, //ATTACK_ID_POISONSTING
  {3, "/gfx/attacks/air_slash.bmp", attackFunction_throwFastAnim}, //ATTACK_ID_AIR_SLASH
  {5, "/gfx/attacks/scratch.bmp", attackFunction_scratch}, //ATTACK_ID_SCRATCH
  {5, "/gfx/attacks/lightning.bmp", attackFunction_lightning}, //ATTACK_ID_LIGHTNING
  {5, "/gfx/attacks/leer.bmp", attackFunction_lightning}, //ATTACK_ID_LEER
  {5, "/gfx/attacks/music_notes_2.bmp", attackFunction_lightning}, //ATTACK_ID_SING
  {5, "/gfx/attacks/rocks.bmp", attackFunction_lightning}, //ATTACK_ID_ROCK_THROW
  {8, "/gfx/attacks/fire.bmp", attackFunction_ember}, // ATTACK_ID_DRAGON_RAGE TODO: currelty just a copy of ember, make it a separate thing
  {5, "/gfx/attacks/scratch.bmp", attackFunction_scratch}, // ATTACK_ID_KARATE_CHOP TODO: currelty just a copy of scratch, make it a separate thing
  {3, "/gfx/attacks/air_slash.bmp", attackFunction_throwFastAnim}, //ATTACK_ID_ICE_BEAM TODO: currelty just a copy of air slash, make it a separate thing
  {5, "/gfx/attacks/scratch.bmp", attackFunction_scratch}, // ATTACK_ID_SAND_TOMB TODO: currelty just a copy of scratch, make it a separate thing
  {0, "", attackFunction_confusion}, // ATTACK_ID_SHADOW_BALL TODO: currelty just a copy of cpnfusion, make it a separate thing
  {5, "/gfx/attacks/scratch.bmp", attackFunction_scratch}, // ATTACK_ID_METAL_CLAW TODO: currelty just a copy of scratch, make it a separate thing
  {5, "/gfx/attacks/scratch.bmp", attackFunction_scratch}, // ATTACK_ID_BITE TODO: currelty just a copy of scratch, make it a separate thing
};


TFT_eSprite playerSprite[] = {TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft)};
TFT_eSprite enemySprite[] = {TFT_eSprite(&tft), TFT_eSprite(&tft)};
uint16_t playerMonsterId = 0;
uint16_t enemyMonsterId = 0;
int16_t loadedAttacks[2] = {-1,-1}; //-1 == no attack loaded
TFT_eSprite attackSprites[2][ATTACK_SPRITE_NUMBER] {
  {TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft),
   TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft)},
  {TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft),
   TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft)},
};

uint8_t attackDodgeBoilerplate(BlowData* blowData, bool playerIsAttacking) {
    if (blowData->lastBlowStatus == LAST_BLOW_FAILED) {
      if (playerIsAttacking) {
        return DRAW_PLAYER_ALIVE | DRAW_ENEMY_DODGE;
      } else {
        return DRAW_PLAYER_ALIVE | DRAW_ENEMY_ALIVE;
      }
    } else {
      if (playerIsAttacking) {
        return DRAW_PLAYER_ALIVE | DRAW_ENEMY_ALIVE;
      } else {
        return DRAW_PLAYER_DODGE | DRAW_ENEMY_ALIVE;
      }
    }
}

uint8_t attackFunction_confusion(DISPLAY_T* display, BlowData* blowData, bool playerIsAttacking, int32_t animTime, bool draw) {
  if (!draw) {
    return attackDodgeBoilerplate(blowData, playerIsAttacking);
  }
  if (animTime >= 2000 || blowData->lastBlowStatus == NEW_BLOW) {
    return DRAW_PLAYER_ALIVE | DRAW_ENEMY_ALIVE;
  }
  if (animTime<1900) {
    tft.invertDisplay(!((animTime / 250) % 2));
  } else {
    tft.invertDisplay(0);
  }
  return 0;
}

uint8_t attackFunction_catch(DISPLAY_T* display, BlowData* blowData, bool playerIsAttacking, int32_t animTime, bool draw) {
  if (!draw) {
    if (blowData->lastBlowStatus == LAST_BLOW_FAILED) {
      return DRAW_PLAYER_ALIVE | DRAW_ENEMY_DODGE;
    } else if (animTime >= 2000 || blowData->lastBlowStatus == NEW_BLOW) {
      return DRAW_PLAYER_ALIVE | DRAW_ENEMY_ALIVE;
    } else {
      return DRAW_PLAYER_ALIVE;
    }
  }
  if (animTime >= 2000 || blowData->lastBlowStatus == NEW_BLOW) {
    return DRAW_PLAYER_ALIVE | DRAW_ENEMY_ALIVE;
  }
  if (animTime<500) {
    int16_t animOffset = (500-animTime) / 4;
    attackSprites[0][0].pushToSprite(display, 200+16-animOffset, 50-32+(animOffset/3), 0x0000);
  } else if (animTime<1000) {
    attackSprites[0][1].pushToSprite(display, 200+16, 50+32, 0x0000);
  } else if (animTime<1500) {
    int16_t animOffset = (1500-animTime) / 8;
    attackSprites[0][0].pushToSprite(display, 200+11+((animTime/250)% 3)*5, 50+32-animOffset, 0x0000);
  } else {
    attackSprites[0][0].pushToSprite(display, 200+11+((animTime/250)% 3)*5, 50+32, 0x0000);
  }
  return 0;
}

uint8_t attackFunction_rareCandy(DISPLAY_T* display, BlowData* blowData, bool playerIsAttacking, int32_t animTime, bool draw) {
  if (!draw) {
    if (blowData->lastBlowStatus == LAST_BLOW_FAILED) {
      return DRAW_PLAYER_ALIVE | DRAW_ENEMY_DODGE;
    } else if (animTime >= 2000 || blowData->lastBlowStatus == NEW_BLOW) {
      return DRAW_PLAYER_ALIVE | DRAW_ENEMY_ALIVE;
    } else {
      return DRAW_PLAYER_ALIVE | DRAW_ENEMY_ALIVE;
    }
  }
  if (animTime >= 2000 || blowData->lastBlowStatus == NEW_BLOW) {
    return DRAW_PLAYER_ALIVE | DRAW_ENEMY_ALIVE;
  }
  if (animTime<1000) {
    int16_t animOffset = (1000-animTime) / 7;
    attackSprites[0][0].pushToSprite(display, 200+16-animOffset, 50+32+(animOffset/3), 0x0000);
  } else {
    attackSprites[0][0].pushToSprite(display, 200+11+((animTime/250)% 3)*5, 50+32, 0x0000);
  }
  return 0;
}

uint8_t attackFunction_ember(DISPLAY_T* display, BlowData* blowData, bool playerIsAttacking, int32_t animTime, bool draw) {
  if (!draw) {
    return attackDodgeBoilerplate(blowData, playerIsAttacking);
  }
  if (animTime >= 2000 || blowData->lastBlowStatus == NEW_BLOW) {
    return DRAW_PLAYER_ALIVE | DRAW_ENEMY_ALIVE;
  }
  uint8_t animCount = attackDataArray[monsterAttackId[playerIsAttacking ? playerMonsterId : enemyMonsterId]].animFrames;
  uint16_t msPerAnimFrame = 2000 / animCount;
  uint8_t animFrame = animCount - 1 - _min(animCount - 1, animTime / msPerAnimFrame);
  if (playerIsAttacking) {
    if (animTime<1000) {
      int16_t animOffset = (1000-animTime) / 7;
      attackSprites[!playerIsAttacking][animFrame].pushToSprite(display, 200+16-animOffset, 50+32+(animOffset/3), 0x0000);
    } else {
      attackSprites[!playerIsAttacking][animFrame].pushToSprite(display, 200+6+((animTime/250)% 3)*10, 50+32, 0x0000);
    }
  } else {
    if (animTime<1000) {
      int16_t animOffset = (1000-animTime) / 7;
      attackSprites[!playerIsAttacking][animFrame].pushToSprite(display, 50+16+animOffset, 100+32-(animOffset/3), 0x0000);
    } else {
      attackSprites[!playerIsAttacking][animFrame].pushToSprite(display, 50 + 6 + ((animTime / 250) % 3) * 10, 100+32, 0x0000);
    }
  }
  return 0; // On draw==true, the return value is ignored;
}

uint8_t attackFunction_throwFastAnim(DISPLAY_T* display, BlowData* blowData, bool playerIsAttacking, int32_t animTime, bool draw) {
  if (!draw) {
    return attackDodgeBoilerplate(blowData, playerIsAttacking);
  }
  if (animTime >= 2000 || blowData->lastBlowStatus == NEW_BLOW) {
    return DRAW_PLAYER_ALIVE | DRAW_ENEMY_ALIVE;
  }
  uint8_t animCount = attackDataArray[monsterAttackId[playerIsAttacking ? playerMonsterId : enemyMonsterId]].animFrames;
  uint8_t animFrame = (animTime / 100) % animCount;
  if (playerIsAttacking) {
    if (animTime<1000) {
      int16_t animOffset = (1000-animTime) / 7;
      attackSprites[!playerIsAttacking][animFrame].pushToSprite(display, 200+16-animOffset, 50+32+(animOffset/3), 0x0000);
    } else {
      attackSprites[!playerIsAttacking][animFrame].pushToSprite(display, 200+6+((animTime/250)% 3)*10, 50+32, 0x0000);
    }
  } else {
    if (animTime<1000) {
      int16_t animOffset = (1000-animTime) / 7;
      attackSprites[!playerIsAttacking][animFrame].pushToSprite(display, 50+16+animOffset, 100+32-(animOffset/3), 0x0000);
    } else {
      attackSprites[!playerIsAttacking][animFrame].pushToSprite(display, 50 + 6 + ((animTime / 250) % 3) * 10, 100+32, 0x0000);
    }
  }
  return 0; // On draw==true, the return value is ignored;
}

uint8_t attackFunction_scratch(DISPLAY_T* display, BlowData* blowData, bool playerIsAttacking, int32_t animTime, bool draw) {
  if (!draw) {
    return attackDodgeBoilerplate(blowData, playerIsAttacking);
  }
  if (animTime >= 2000 || blowData->lastBlowStatus == NEW_BLOW) {
    return DRAW_PLAYER_ALIVE | DRAW_ENEMY_ALIVE;
  }
  uint8_t animCount = attackDataArray[monsterAttackId[playerIsAttacking ? playerMonsterId : enemyMonsterId]].animFrames;
  uint8_t animFrame = (animTime / 100) % animCount;
  if (playerIsAttacking) {
    attackSprites[!playerIsAttacking][animFrame].pushToSprite(display, 200+6+((animTime/250)% 3)*10, 50+32, 0x0000);
  } else {
    attackSprites[!playerIsAttacking][animFrame].pushToSprite(display, 50 + 6 + ((animTime / 250) % 3) * 10, 100+32, 0x0000);
  }
  return 0; // On draw==true, the return value is ignored;
}

uint8_t attackFunction_lightning(DISPLAY_T* display, BlowData* blowData, bool playerIsAttacking, int32_t animTime, bool draw) {
  if (!draw) {
    return attackDodgeBoilerplate(blowData, playerIsAttacking);
  }
  if (animTime >= 2000 || blowData->lastBlowStatus == NEW_BLOW) {
    return DRAW_PLAYER_ALIVE | DRAW_ENEMY_ALIVE;
  }
  uint8_t animCount = attackDataArray[monsterAttackId[playerIsAttacking ? playerMonsterId : enemyMonsterId]].animFrames;
  uint8_t animFrame = (animTime / 100) % animCount;
  if (playerIsAttacking) {
    attackSprites[!playerIsAttacking][animFrame].pushToSprite(display, 200+16, 50+16, 0x0000);
  } else {
    attackSprites[!playerIsAttacking][animFrame].pushToSprite(display, 50+16, 100+16, 0x0000);
  }
  return 0; // On draw==true, the return value is ignored;
}

void loadAttackSprites(uint16_t attackId, uint8_t slot) {
  Serial.print("Loading attack ");
  Serial.print(attackId);
  Serial.print(" into slot ");
  Serial.println(slot);
  if (loadedAttacks[slot] == attackId) {
    Serial.print("Attack ");
    Serial.print(attackId);
    Serial.print(" already loaded into slot ");
    Serial.print(slot);
    Serial.println(".");
    return;
  }
  loadedAttacks[slot] = attackId;
  TFT_eSprite* attackSpriteRefs[] = {
    &attackSprites[slot][0],
    &attackSprites[slot][1],
    &attackSprites[slot][2],
    &attackSprites[slot][3],
    &attackSprites[slot][4],
    &attackSprites[slot][5],
    &attackSprites[slot][6],
    &attackSprites[slot][7],
  };
  loadBmpAnim(attackSpriteRefs, attackDataArray[attackId].gfxPath, attackDataArray[attackId].animFrames, slot==1 ? FLIPPED_H : 0);
}

void drawCombat(DISPLAY_T* display, BlowData* blowData, uint8_t numberOfAttacks, AttackFunctionType attackFunctions[], bool isCatch) {
  #ifdef SHOW_REMAINING_SECONDS
    display->print(timeLeft, 10);
  #endif
  bool isPlayerTurn = !(blowData->blowCount % 2);
  uint8_t currentAttackFunction = blowData->blowCount % numberOfAttacks;
  uint8_t drawCombattantSprites;
  int32_t animTime = ((int32_t)blowData->ms) - ((int32_t)blowData->blowEndMs);
  if (attackFunctions[currentAttackFunction] != NULL && (blowData->blowEndMs > 0)) {
    uint8_t attackFunctionId = (blowData->lastBlowStatus==LAST_BLOW_FAILED ? blowData->blowCount + 1 : blowData->blowCount) % numberOfAttacks;
    drawCombattantSprites = attackFunctions[attackFunctionId](display, blowData, isPlayerTurn, animTime, false);
  } else {
    drawCombattantSprites = DRAW_PLAYER_ALIVE | DRAW_ENEMY_ALIVE;
  }

  uint8_t playerAnimSprite = isCatch ? _min(3, animTime / 100) : 0;

  if (drawCombattantSprites & DRAW_PLAYER_ALIVE || (drawCombattantSprites & DRAW_PLAYER_DODGE && animTime >= 2000)) {
    playerSprite[playerAnimSprite].pushToSprite(display, 50, 100);
  } else if (drawCombattantSprites & DRAW_PLAYER_DODGE) {
    int16_t xOffset = _min(50, animTime / 10);
    playerSprite[playerAnimSprite].pushToSprite(display, 50 - xOffset, 100);
  }
  uint16_t hpPerFail = 100 / 10;
  if (blowData->isLongBlows) {
    drawProgressBar(display, _max(1, 100 - hpPerFail * blowData->fails), 100, 10, 158, 100, 10);
  }
  /*if ((drawCombattantSprites & DRAW_PLAYER_DEAD) || (altKillBitmap == NULL && drawCombattantSprites & DRAW_PLAYER_ALTERNATE)) {
    drawBitmap(display, 1, GAME_DRAW_Y, explosion_bmp);
  }
  if (altKillBitmap != NULL && drawCombattantSprites & DRAW_PLAYER_ALTERNATE) {
    drawBitmap(display, 1, GAME_DRAW_Y, altKillBitmap);
  }*/
  if (drawCombattantSprites & DRAW_ENEMY_ALIVE || (drawCombattantSprites & DRAW_ENEMY_DODGE && animTime >= 2000)) {
    enemySprite[(blowData->ms / 500) % 2].pushToSprite(display, 200, 50);
  } else if (drawCombattantSprites & DRAW_ENEMY_DODGE) {
    int16_t xOffset = _min(50, animTime / 10);
    enemySprite[(blowData->ms / 500) % 2].pushToSprite(display, 200 + xOffset, 50);
  }
  uint16_t hpPerCycle = 100 / (blowData->isLongBlows ? LONG_BLOW_NUMBER_MAX : SHORT_BLOW_NUMBER_MAX);
  drawProgressBar(display, hpPerCycle*2*(((blowData->isLongBlows ? LONG_BLOW_NUMBER_MAX : SHORT_BLOW_NUMBER_MAX)-blowData->blowCount + 1)/2), 100, 200, 20, 100, 10);
  /*if (drawCombattantSprites & DRAW_ENEMY_DEAD || (altKillBitmap == NULL && drawCombattantSprites & DRAW_ENEMY_ALTERNATE)) {
    drawBitmap(display, SCREEN_WIDTH-1, GAME_DRAW_Y, explosion_bmp, BITMAP_HRIGHT_ALIGN);
  }
  if (altKillBitmap != NULL && drawCombattantSprites & DRAW_ENEMY_ALTERNATE) {
    drawBitmap(display, SCREEN_WIDTH-1, GAME_DRAW_Y, altKillBitmap, BITMAP_HRIGHT_ALIGN);
  }*/
  if (attackFunctions[currentAttackFunction] != NULL && (blowData->blowEndMs > 0)) {
    attackFunctions[blowData->blowCount % numberOfAttacks](display, blowData, isPlayerTurn, animTime, true);
  }
}

AttackFunctionType getAttackFunction(uint16_t monsterId) {
  return attackDataArray[monsterAttackId[monsterId]].attackFunction;
}

inline void loadPlayerMonsterIdFromSD() {
  uint8_t read = 0;
  //read = readIntFromFile("/playerMonsterId.txt");
  read = prefs.getInt("playerMonsterId", 1);
  Serial.print(F("Loaded monster id: "));
  Serial.println(read);
  playerMonsterId = max(1, read % (TOTAL_MONSTER_NUMBER+1));
  Serial.print(F("Corrected to: "));
  Serial.println(playerMonsterId);
}

inline void savePlayerMonsterIdToSD() {
  Serial.print(F("Saving monster id: "));
  Serial.println(playerMonsterId);
  prefs.putInt("playerMonsterId", playerMonsterId);
  //writeIntToFile("/playerMonsterId.txt", playerMonsterId);
}

int8_t lastCycleMonsterSelected = -1;
void drawGameLongBlows_MonsterCombat(DISPLAY_T* display, BlowData* blowData) {
  if (playerMonsterId == 0) {
    loadPlayerMonsterIdFromSD();
  }
  if (enemyMonsterId == 0 || blowData->cycleNumber > lastCycleMonsterSelected) {
    TFT_eSprite* playerSpriteRefs[] = { // Reload player anim at the beginning of the round
      &playerSprite[0],
      &playerSprite[1]
    };
    loadBmpAnim(playerSpriteRefs, monsterImagePath[playerMonsterId] + "/back.bmp", 1, 0);
    loadAttackSprites(monsterAttackId[playerMonsterId], 0);

    lastCycleMonsterSelected = blowData->cycleNumber;
    Serial.print(F("Choosing new enemy: "));
    enemyMonsterId = random(1, TOTAL_MONSTER_NUMBER + 1);
    Serial.println(enemyMonsterId);
    TFT_eSprite* enemySpriteRefs[] = {
      &enemySprite[0],
      &enemySprite[1]
    };
    loadBmpAnim(enemySpriteRefs, monsterImagePath[enemyMonsterId] + "/anim_front.bmp", 2);
    loadAttackSprites(monsterAttackId[enemyMonsterId], 1);
  }
  AttackFunctionType attackFunctions[2];
  attackFunctions[0] = getAttackFunction(playerMonsterId);
  attackFunctions[1] = getAttackFunction(enemyMonsterId);
  drawCombat(display, blowData, 2, attackFunctions, false);
}

int8_t catchCheckForCycle = -1;
int8_t catchCheckForBlow = -1;
uint16_t playerEvolutionTarget;
bool catchMonsterModeEvolution = false;
void drawGameShortBlows_CatchMonster(DISPLAY_T* display, BlowData* blowData) {
  if (blowData->cycleNumber > catchCheckForCycle) {
    bool enemyCanBeCaught = isBasicMonster(enemyMonsterId);
    playerEvolutionTarget = getMonsterEvolvesTo(playerMonsterId);
    TFT_eSprite* playerSpriteRefs[] = {
      &playerSprite[0],
      &playerSprite[1],
      &playerSprite[2],
      &playerSprite[3],
    };
    loadBmpAnim(playerSpriteRefs, monsterImagePath[playerMonsterId] + "/anim_front.bmp", 2);
    if (playerEvolutionTarget != 0 && enemyCanBeCaught) {
      while (true) {
        display->fillSprite(TFT_BLACK);
        display->fillRect(15,55,130,130,TFT_BLUE);
        playerSprite[(blowData->ms/250) % 2].pushToSprite(display, 48, 88);
        display->fillRect(160,55,130,130,TFT_BLUE);
        enemySprite[(blowData->ms/250) % 2].pushToSprite(display, 193, 88);
        display->pushSprite(0,0);
        if (touch.pressed()) {
          Serial.println("Touch pressed");
          if (isTouchInZone(15,55,130,130)) {
            Serial.println("Selection Evolution");
            catchMonsterModeEvolution = true;
            break;
          } else if (isTouchInZone(145,55,130,130)) {
            Serial.println("Selection Catch");
            catchMonsterModeEvolution = false;
            break;
          }
        }
      }
    } else if (playerEvolutionTarget != 0) {
      Serial.println("Defaulting to evolution");
      catchMonsterModeEvolution = true;
    } else {
      Serial.println("Defaulting to catch");
      if (!enemyCanBeCaught) {
        Serial.println("Missingno!");
        enemyMonsterId = 0; //MissingNo easteregg if player cannot evolve and enemy cannot be caught.
      }
      catchMonsterModeEvolution = false;
    }
    if (catchMonsterModeEvolution) {
      loadAttackSprites(ATTACK_ID_RARE_CANDY, 0);
    } else {
      loadAttackSprites(ATTACK_ID_BALL, 0);
    }
    loadBmpAnim(playerSpriteRefs, TRAINER_ANIM_PATH, 4);
    TFT_eSprite* enemySpriteRefs[] = {
      &enemySprite[0],
      &enemySprite[1]
    };
    loadBmpAnim(enemySpriteRefs, monsterImagePath[catchMonsterModeEvolution ? playerMonsterId : enemyMonsterId] + "/anim_front.bmp", 2);
  }

  if (catchCheckForBlow && blowData->blowCount == SHORT_BLOW_NUMBER_MAX) {
      catchCheckForBlow = blowData->blowCount;
      if (catchMonsterModeEvolution) {
        playerMonsterId = playerEvolutionTarget;
      } else {
        if (enemyMonsterId==0) {
          while (!isBasicMonster(enemyMonsterId)) {
            enemyMonsterId = random(1, TOTAL_MONSTER_NUMBER + 1);
          }
        }
        playerMonsterId = enemyMonsterId;
      }
      savePlayerMonsterIdToSD();
      TFT_eSprite* playerSpriteRefs[] = {
        &playerSprite[0],
        &playerSprite[1]
      };
      loadBmpAnim(playerSpriteRefs, monsterImagePath[playerMonsterId] + "/anim_front.bmp", 2);
    }

  if (blowData->blowCount == SHORT_BLOW_NUMBER_MAX) { // If is monster evolution, display evolved monster on last round
    playerSprite[(blowData->ms/250)%2].pushToSprite(display, SCREEN_WIDTH/2-16, SCREEN_HEIGHT/2-16, 0x0000);
    return;
  }

  catchCheckForCycle = blowData->cycleNumber;
  AttackFunctionType attackFunctions[1];
  if (catchMonsterModeEvolution) {
    attackFunctions[0] = attackFunction_rareCandy;
  } else {
    attackFunctions[0] = attackFunction_catch;
  }
  drawCombat(display, blowData, 1, attackFunctions, true);
}


void initGames() {
  prefs.begin("game-monsters");
  for (int8_t i = 0; i<4; i++) {
    playerSprite[i].createSprite(64, 64);
    playerSprite[i].setColorDepth(16); //redundant
  }
  for (int8_t i = 0; i<2; i++) {
    enemySprite[i].createSprite(64, 64);
    enemySprite[i].setColorDepth(16); //redundant
    for (int8_t j = 0; j<ATTACK_SPRITE_NUMBER; j++) {
      attackSprites[i][j].createSprite(32,32);
    }
  }
}

bool drawShortBlowGame(uint8_t index, DISPLAY_T* display, BlowData* blowData) {
  index = index % NUM_SHORT_BLOW_GAMES;
  index = 0;
  switch (index) {
    case 0:
      drawGameShortBlows_CatchMonster(display, blowData);
      //drawGameShortBlows_Ballon(display, blowData);
      break;
    case 1:
      //drawGameShortBlows_ShipCombat(display, blowData);
      break;
    case 2:
      //drawGameShortBlows_Cannon(display, blowData);
      break;
  }
  return false;
}

bool drawLongBlowGame(uint8_t index, DISPLAY_T* display, BlowData* blowData) {
  index = index % NUM_LONG_BLOW_GAMES;
  index = 2;
  switch (index) {
    /*case 255: // NOT USED
      drawGame_Ship(display, blowData);
      break;*/
    case 0:
      //drawGameLongBlows_MageCombat(display, blowData);
      break;
    case 1:
      //drawGameLongBlows_Snowman(display, blowData);
      break;
    case 2:
      drawGameLongBlows_MonsterCombat(display, blowData);
      break;
  }
  return false;
}