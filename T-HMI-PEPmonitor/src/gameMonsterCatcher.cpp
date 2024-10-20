#include "gameMonsterCatcher.h"

String monsterCatcherGamePath;
String monsterCatcherGameIniPath;

TFT_eSprite playerSprite[] = {TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft)};
TFT_eSprite enemySprite[] = {TFT_eSprite(&tft), TFT_eSprite(&tft)};
TFT_eSprite safariBushSprite[] {TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft)};
TFT_eSprite ballCaughtIndicator = TFT_eSprite(&tft);
MonsterData playerMonsterData;
MonsterData enemyMonsterData;
uint8_t monsterLevels[MAX_MONSTER_NUMBER];
String loadedAttacks[2] = {"",""}; //-1 == no attack loaded
TFT_eSprite attackSprites[2][ATTACK_SPRITE_NUMBER] {
  {TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft),
   TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft)},
  {TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft),
   TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft)},
};

void loadPlayerMonsterId(String* errorMessage);
void savePlayerMonsterId();


uint16_t getRandomPreviouslyCaughtMonster() {
  uint16_t monsterId = 0;
  while (true) {
    monsterId = random(0, MAX_MONSTER_NUMBER);
    if (monsterLevels[monsterId] > 0) {
      return monsterId;
    }
  }
}

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
  uint8_t animCount = _max(1, playerIsAttacking ? playerMonsterData.attack.animFrames : enemyMonsterData.attack.animFrames);
  uint16_t msPerAnimFrame = _max(1, 2000 / animCount);
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
  uint8_t animCount = _max(1, playerIsAttacking ? playerMonsterData.attack.animFrames : enemyMonsterData.attack.animFrames);
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
  uint8_t animCount = _max(1, playerIsAttacking ? playerMonsterData.attack.animFrames : enemyMonsterData.attack.animFrames);
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
  uint8_t animCount = _max(1, playerIsAttacking ? playerMonsterData.attack.animFrames : enemyMonsterData.attack.animFrames);
  uint8_t animFrame = (animTime / 100) % animCount;
  if (playerIsAttacking) {
    attackSprites[!playerIsAttacking][animFrame].pushToSprite(display, 200+16, 50+16, 0x0000);
  } else {
    attackSprites[!playerIsAttacking][animFrame].pushToSprite(display, 50+16, 100+16, 0x0000);
  }
  return 0; // On draw==true, the return value is ignored;
}

void loadAttackSprites(AttackData* attack, uint8_t slot) {
  Serial.print("Loading attack ");
  Serial.print(attack->identifier);
  Serial.print(" into slot ");
  Serial.println(slot);
  if (loadedAttacks[slot] == attack->identifier) {
    Serial.print("Attack ");
    Serial.print(attack->identifier);
    Serial.print(" already loaded into slot ");
    Serial.print(slot);
    Serial.println(".");
    return;
  }
  loadedAttacks[slot] = attack->identifier;
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
  loadBmpAnim(attackSpriteRefs, monsterCatcherGamePath + attack->imagePath, attack->animFrames, slot==1 ? FLIPPED_H : 0);
}

void loadAttackSprites(String attackId, uint8_t slot, String* errorMessage) {
  AttackData attackData;
  loadAttackData(monsterCatcherGameIniPath, &attackData, attackId, errorMessage);
  if (!errorMessage->isEmpty()) {
    return;
  }
  loadAttackSprites(&attackData, slot);
}

void drawCombat(DISPLAY_T* display, BlowData* blowData, uint8_t numberOfAttacks, AttackFunctionType attackFunctions[], bool isCatch, String* errorMessage) {
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

  uint16_t hpPerFail = 100 / 10;
  uint16_t hp = 100 - hpPerFail * blowData->fails;
  if (hp<=0 && (animTime >= 2000 || blowData->lastBlowStatus == NEW_BLOW)) {
      hp = 100;
      blowData->fails = 0;
      loadMonsterData(monsterCatcherGameIniPath, &playerMonsterData, getRandomPreviouslyCaughtMonster(), errorMessage);
      TFT_eSprite* playerSpriteRefs[] = { // Reload player anim at the beginning of the round
        &playerSprite[0],
        &playerSprite[1]
      };
      loadBmpAnim(playerSpriteRefs, monsterCatcherGamePath + playerMonsterData.imagePath, 1, 0);
      loadAttackSprites(&(playerMonsterData.attack), 0);
      savePlayerMonsterId();
  }
  if (hp<=0) {
    int16_t yOffset = _min(150, animTime / 3);
    playerSprite[playerAnimSprite].pushToSprite(display, 50, 100 + yOffset);
  } else if (drawCombattantSprites & DRAW_PLAYER_ALIVE || (drawCombattantSprites & DRAW_PLAYER_DODGE && animTime >= 2000)) {
    playerSprite[playerAnimSprite].pushToSprite(display, 50, 100);
  } else if (drawCombattantSprites & DRAW_PLAYER_DODGE) {
    int16_t xOffset = _min(50, animTime / 10);
    playerSprite[playerAnimSprite].pushToSprite(display, 50 - xOffset, 100);
  }
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
  if (monsterLevels[enemyMonsterData.id] > 0) {
    ballCaughtIndicator.pushToSprite(display, 190195, 50);
  }
  display->setCursor(200, 33);
  display->setTextSize(2);
  display->print(enemyMonsterData.name);
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

void loadPlayerMonsterId(String* errorMessage) {
  uint8_t read = 0;
  read = prefs.getInt("playerMonsterId", 1);
  Serial.print(F("Loaded monster ID: "));
  Serial.println(read);
  uint16_t monsterCount = getMonsterCount(monsterCatcherGameIniPath, errorMessage);
  loadMonsterData(monsterCatcherGameIniPath, &playerMonsterData, (uint16_t)_max(1, read % (monsterCount+1)), errorMessage);
  Serial.print(F("Corrected to: "));
  Serial.println(playerMonsterData.id);
}

void savePlayerMonsterId() {
  #ifndef DEBUG_DISABLE_SAVING
    Serial.print(F("Saving monster id: "));
    Serial.println(playerMonsterData.id);
    if (monsterLevels[playerMonsterData.id] == 0) {
      monsterLevels[playerMonsterData.id] = 1;
      prefs.putBytes("levels", monsterLevels, MAX_MONSTER_NUMBER);
    }
    prefs.putInt("playerMonsterId", playerMonsterData.id);
  #else
    Serial.println("SAVING MONSTER ID DISABLED. TO ENABLE, REMOVE DEBUG_DISABLE_SAVING!");
  #endif
}

int8_t lastCycleMonsterSelected = -1;
// Monster Combat game
void drawLongBlowGame_monsterCatcher(DISPLAY_T* display, BlowData* blowData, String* errorMessage) {
  if (playerMonsterData.id == 0) {
    loadPlayerMonsterId(errorMessage);
  }
  if (enemyMonsterData.id == 0 || blowData->cycleNumber > lastCycleMonsterSelected) {
    TFT_eSprite* playerSpriteRefs[] = { // Reload player anim at the beginning of the round
      &playerSprite[0],
      &playerSprite[1]
    };
    loadBmpAnim(playerSpriteRefs, monsterCatcherGamePath + playerMonsterData.imagePath + "/back.bmp", 1, 0);
    loadAttackSprites(&(playerMonsterData.attack), 0);

    lastCycleMonsterSelected = blowData->cycleNumber;
    Serial.print(F("Choosing new enemy: "));
    loadMonsterData(monsterCatcherGameIniPath, &enemyMonsterData, getRandomMonsterId(monsterCatcherGameIniPath, errorMessage), errorMessage);
    Serial.println(enemyMonsterData.id);
    TFT_eSprite* enemySpriteRefs[] = {
      &enemySprite[0],
      &enemySprite[1]
    };
    loadBmpAnim(enemySpriteRefs, monsterCatcherGamePath + enemyMonsterData.imagePath + "/anim_front.bmp", 2);
    loadAttackSprites(&(enemyMonsterData.attack), 1);
  }
  AttackFunctionType attackFunctions[2];
  attackFunctions[0] = playerMonsterData.attack.attackFunction;
  attackFunctions[1] = enemyMonsterData.attack.attackFunction;
  drawCombat(display, blowData, 2, attackFunctions, false, errorMessage);
}

int8_t catchCheckForCycle = -1;
int8_t catchCheckForBlow = -1;
uint16_t playerEvolutionTarget;
bool catchMonsterModeEvolution = false;
// Monster Catching/Evolving game
void drawShortBlowGame_monsterCatcher(DISPLAY_T* display, BlowData* blowData, String* errorMessage) {
  if (blowData->cycleNumber > catchCheckForCycle) {
    bool enemyCanBeCaught = enemyMonsterData.isBasicMonster;
    playerEvolutionTarget = playerMonsterData.evolvesTo;
    TFT_eSprite* playerSpriteRefs[] = {
      &playerSprite[0],
      &playerSprite[1],
      &playerSprite[2],
      &playerSprite[3],
    };
    loadBmpAnim(playerSpriteRefs, monsterCatcherGamePath + playerMonsterData.imagePath + "/anim_front.bmp", 2);
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
        loadMonsterData(monsterCatcherGameIniPath, &enemyMonsterData, 0, errorMessage); //MissingNo easteregg if player cannot evolve and enemy cannot be caught.
      }
      catchMonsterModeEvolution = false;
    }
    if (catchMonsterModeEvolution) {
      enemyMonsterData = playerMonsterData;
      loadAttackSprites(ATTACK_IDENTIFIER_RARE_CANDY, 0, errorMessage);
    } else {
      loadAttackSprites(ATTACK_IDENTIFIER_CATCH_BALL, 0, errorMessage);
    }
    loadBmpAnim(playerSpriteRefs, monsterCatcherGamePath + TRAINER_ANIM_PATH, 4);
    TFT_eSprite* enemySpriteRefs[] = {
      &enemySprite[0],
      &enemySprite[1]
    };
    loadBmpAnim(enemySpriteRefs, monsterCatcherGamePath + enemyMonsterData.imagePath + "/anim_front.bmp", 2);
  }

  if (catchCheckForBlow && blowData->blowCount == SHORT_BLOW_NUMBER_MAX) {
      catchCheckForBlow = blowData->blowCount;
      if (catchMonsterModeEvolution) {
        loadMonsterData(monsterCatcherGameIniPath, &playerMonsterData, playerEvolutionTarget, errorMessage);
      } else {
        if (enemyMonsterData.id==0) {
          while (!enemyMonsterData.isBasicMonster) {
            loadMonsterData(monsterCatcherGameIniPath, &enemyMonsterData, getRandomMonsterId(monsterCatcherGameIniPath, errorMessage), errorMessage);
          }
        }
        loadMonsterData(monsterCatcherGameIniPath, &playerMonsterData, enemyMonsterData.id, errorMessage);
      }
      savePlayerMonsterId();
      TFT_eSprite* playerSpriteRefs[] = {
        &playerSprite[0],
        &playerSprite[1]
      };
      loadBmpAnim(playerSpriteRefs, monsterCatcherGamePath + playerMonsterData.imagePath + "/anim_front.bmp", 2);
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
  drawCombat(display, blowData, 1, attackFunctions, true, errorMessage);
}

void loadBushes(uint8_t nr) {
  Serial.print("Loading bushes ");
  Serial.println(nr);
  for (int8_t i = 0; i<4; i++) {
    String setNr = String(nr);
    String filePath = String(i+1);
    filePath = monsterCatcherGamePath + "gfx/safari/Bush" + setNr + "_" + filePath + ".bmp";
    loadBmp(&safariBushSprite[i], filePath);
  }
}

int16_t jumpCountLoaded = -1;
bool winLoaded = false;
// Safari Zone game
void drawTrampolineGame_monsterCatcher(DISPLAY_T* display, JumpData* jumpData, String* errorMessage) {
  if (!winLoaded && jumpData->msLeft<0) {
    Serial.println("Win hit");
    winLoaded = true;
    loadMonsterData(monsterCatcherGameIniPath, &playerMonsterData, getSafariMonster(monsterCatcherGameIniPath,_min(3, 1 + (jumpData->jumpCount / 100)), errorMessage), errorMessage);
    TFT_eSprite* playerSpriteRefs[] = {
      &playerSprite[0],
      &playerSprite[1],
      &playerSprite[2],
      &playerSprite[3],
    };
    savePlayerMonsterId();
    loadBmpAnim(playerSpriteRefs, monsterCatcherGamePath + playerMonsterData.imagePath + "/anim_front.bmp", 2);
  }
  if (jumpData->msLeft<0) {
    playerSprite[(jumpData->ms/250)%2].pushToSprite(display, SCREEN_WIDTH/2-16, SCREEN_HEIGHT/2-16, 0x0000);
    return;
  }
  if (jumpData->jumpCount>jumpCountLoaded) {
    Serial.println("Loading new bushes");
    jumpCountLoaded += 50;
    loadBushes(_min(10,_max(1,1+(jumpData->jumpCount/50))));
  }
  int16_t bushYShift = jumpData->currentlyJumping ? 10*((jumpData->ms / 500) % 2) : 0;
  safariBushSprite[0].pushToSprite(display, 180, 80 + bushYShift, 0x00);
  safariBushSprite[0].pushToSprite(display, 80, 80 + bushYShift, 0x00);
  safariBushSprite[1].pushToSprite(display, 120, 101 + bushYShift, 0x00);
  safariBushSprite[2].pushToSprite(display, 170, 123 + bushYShift, 0x00);
  safariBushSprite[3].pushToSprite(display, 115, 141 + bushYShift, 0x00);
  safariBushSprite[3].pushToSprite(display, 100, 141 + bushYShift, 0x00);
  safariBushSprite[3].pushToSprite(display, 200, 141 + bushYShift, 0x00);
}


void initGames_monsterCatcher(String gamePath, GameConfig* gameConfig, String* errorMessage) {
  Serial.print("Game path: ");
  Serial.println(gamePath);
  monsterCatcherGamePath = gamePath;
  monsterCatcherGameIniPath = gamePath + "gameconfig.ini";
  prefs.begin(gameConfig->prefsNamespace.c_str());
  if (prefs.getBytes("levels", monsterLevels, MAX_MONSTER_NUMBER) == 0) {
    for (int16_t i=0; i<MAX_MONSTER_NUMBER; i++) {
      monsterLevels[i] = 0;
    }
    monsterLevels[1] = 1;
  }
  for (int8_t i=0; i<4; i++) {
    playerSprite[i].createSprite(64, 64);
    playerSprite[i].setColorDepth(16); //redundant
  }
  for (int8_t i=0; i<2; i++) {
    enemySprite[i].createSprite(64, 64);
    enemySprite[i].setColorDepth(16); //redundant
    for (int8_t j = 0; j<ATTACK_SPRITE_NUMBER; j++) {
      attackSprites[i][j].createSprite(32,32);
    }
  }
  safariBushSprite[0].createSprite(124,96);
  safariBushSprite[1].createSprite(104,75);
  safariBushSprite[2].createSprite(75,58);
  safariBushSprite[3].createSprite(59,40);
  for (int8_t i = 0; i<4; i++) {
    safariBushSprite[i].setColorDepth(16);
  }
  loadBushes(1);
  ballCaughtIndicator.createSprite(7,7);
  ballCaughtIndicator.setColorDepth(16);
  loadBmp(&ballCaughtIndicator, monsterCatcherGamePath + "gfx/interface/ball_caught_indicator.bmp");
}