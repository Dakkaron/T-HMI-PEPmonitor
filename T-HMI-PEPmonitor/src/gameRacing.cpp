#include "gameRacing.h"

#define TURN_STRAIGHT 0
#define TURN_LEFT 1
#define TURN_RIGHT 2
#define TURN_HARD_LEFT 3
#define TURN_HARD_RIGHT 4

uint8_t nitro;
String racerGamePath;
String racerGameIniPath;

TFT_eSprite playerCarSprite[] =    {TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft),
                                    TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft)};
TFT_eSprite pitstopCarSprite[] =   {TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft)};
TFT_eSprite pitstopWheelSprite =    TFT_eSprite(&tft);
TFT_eSprite enemyCarSprite[][4] = {{TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft)},
                                   {TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft)},
                                   {TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft)},
                                   {TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft)}};
TFT_eSprite nitroLSprite =          TFT_eSprite(&tft);
TFT_eSprite nitroSSprite =          TFT_eSprite(&tft);
TFT_eSprite nitroEffectSprite[] =  {TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft)};

void initGames_racing(String gamePath, GameConfig* gameConfig, String* errorMessage) {
  Serial.print("Game path: ");
  Serial.println(gamePath);
  racerGamePath = gamePath;
  racerGameIniPath = gamePath + "gameconfig.ini";
  prefs.begin(gameConfig->prefsNamespace.c_str());
  nitro = prefs.getUInt("nitro", 0);
  for (int8_t i=0; i<10; i++) {
    playerCarSprite[i].createSprite(70, 37);
  }
  for (int8_t i=0; i<5; i++) {
    nitroEffectSprite[i].createSprite(58,8);
  }
  for (int8_t i=0; i<4; i++) {
    pitstopCarSprite[i].createSprite(128, 60);
    loadBmp(&pitstopCarSprite[i], racerGamePath + "gfx/car"+i+"_top.bmp", FLIPPED_H);

    enemyCarSprite[i][0].createSprite(70, 37);
    enemyCarSprite[i][1].createSprite(53, 28);
    enemyCarSprite[i][2].createSprite(35, 19);
    enemyCarSprite[i][3].createSprite(18, 10);
    pitstopWheelSprite.createSprite(17,18);
    loadBmp(&pitstopWheelSprite, racerGamePath + "gfx/wheel.bmp");
    loadBmp(&enemyCarSprite[i][0], racerGamePath + "gfx/car"+i+"_0_0.bmp");
    loadBmp(&enemyCarSprite[i][1], racerGamePath + "gfx/car"+i+"_0.75.bmp");
    loadBmp(&enemyCarSprite[i][2], racerGamePath + "gfx/car"+i+"_0.5.bmp");
    loadBmp(&enemyCarSprite[i][3], racerGamePath + "gfx/car"+i+"_0.25.bmp");
  }
  nitroLSprite.createSprite(54,96);
  nitroSSprite.createSprite(17,30);
  TFT_eSprite* nitroSpriteRefs[] = { // Reload player anim at the beginning of the round
    &nitroEffectSprite[0],
    &nitroEffectSprite[1],
    &nitroEffectSprite[2],
    &nitroEffectSprite[3],
    &nitroEffectSprite[4],
  };
  loadBmpAnim(nitroSpriteRefs, racerGamePath + "gfx/nitroEffect.bmp", 5);
  loadBmp(&nitroLSprite, racerGamePath + "gfx/nitroL.bmp");
  loadBmp(&nitroSSprite, racerGamePath + "gfx/nitroS.bmp");
  loadBmp(&playerCarSprite[0], racerGamePath + "gfx/car0_0_0.bmp");
  loadBmp(&playerCarSprite[1], racerGamePath + "gfx/car0_0_1.bmp");
  loadBmp(&playerCarSprite[2], racerGamePath + "gfx/car0_1_0.bmp");
  loadBmp(&playerCarSprite[3], racerGamePath + "gfx/car0_1_1.bmp");
  loadBmp(&playerCarSprite[4], racerGamePath + "gfx/car0_1_0.bmp", FLIPPED_H);
  loadBmp(&playerCarSprite[5], racerGamePath + "gfx/car0_1_1.bmp", FLIPPED_H);
  loadBmp(&playerCarSprite[6], racerGamePath + "gfx/car0_2_0.bmp");
  loadBmp(&playerCarSprite[7], racerGamePath + "gfx/car0_2_1.bmp");
  loadBmp(&playerCarSprite[8], racerGamePath + "gfx/car0_2_0.bmp", FLIPPED_H);
  loadBmp(&playerCarSprite[9], racerGamePath + "gfx/car0_2_1.bmp", FLIPPED_H);
}

uint8_t currentTurnType;
int16_t roadXOffset = 0;
uint8_t enemySpriteId = 0;
void calculateRoad(uint32_t y, uint32_t speed, unsigned long ms, int32_t* x, int32_t* w, int32_t* roadYOffset) {
  *w = 7*(y*y+10)/1000;
  *x = 160 - *w/2;
  int32_t yInv = 200-y;
  *x += roadXOffset*(yInv*yInv)/10000;
  *roadYOffset = (((yInv*yInv) + ms*speed)/900);
}

void drawEnemyCar(DISPLAY_T* display, int32_t x, int32_t y, int32_t w, int32_t enemyY) {
  // Draw enemy car
  if (y == enemyY) {
    uint8_t enemySpriteLevel;
    if (y>150) {
      enemySpriteLevel = 0;
    } else if (y>100) {
      enemySpriteLevel = 1;
    } else if (y>50) {
      enemySpriteLevel = 2;
    } else {
      enemySpriteLevel = 3;
    }
    TFT_eSprite* sprite = &enemyCarSprite[enemySpriteId][enemySpriteLevel];
    if (currentTurnType == TURN_RIGHT || currentTurnType == TURN_HARD_RIGHT) {
      sprite->pushToSprite(display, x + w - w/7 - sprite->width()/2, y - sprite->height(), 0);
    } else {
      sprite->pushToSprite(display, x + w/7 - sprite->width()/2, y - sprite->height(), 0);
    }
  }
}

void drawRace(DISPLAY_T* display, BlowData* blowData, int32_t animTime) {
  int32_t enemyY = -1;
  int32_t enemy2Y = -1;
  int32_t x,w,roadYOffset;
  int32_t speed = 25;
  if (animTime <= 2000 && (blowData->lastBlowStatus & LAST_BLOW_FAILED)) {
    enemyY = 50 + (150*(2000-animTime) / 2000);
  } else if (animTime <= 2000) {
    enemyY = animTime / 10;
    if (nitro>0) {
      enemy2Y = enemyY + 40;
      speed = 40;
    }
  }
  switch (currentTurnType) {
    case TURN_LEFT:
      roadXOffset -= roadXOffset>-33 ? 1 : 0;
      break;
    case TURN_RIGHT:
      roadXOffset += roadXOffset<33 ? 1 : 0;
      break;
    case TURN_HARD_LEFT:
      roadXOffset -= roadXOffset>-83 ? 2 : 0;
      break;
    case TURN_HARD_RIGHT:
      roadXOffset += roadXOffset<83 ? 2 : 0;
      break;
  }
  for (int32_t y=50;y<190;y++) {
    calculateRoad(y, speed, blowData->ms, &x, &w, &roadYOffset);
    // Draw road surface
    display->drawFastHLine(x, y, w, (roadYOffset/6) % 2 ? 0x5AEB : 0x8410);
    // Draw embankment
    display->drawFastHLine(x-w/10, y, w/10, (roadYOffset/3) % 2 ? 0xf800: 0xffff);
    display->drawFastHLine(x+w, y, w/10, (roadYOffset/3) % 2 ? 0xf800: 0xffff);
    // Draw center line
    if ((roadYOffset/4) % 2) {
      display->drawFastHLine(x+w/2-w/20, y, w/10, 0xffff);
    }

    // Draw wall
    display->drawFastVLine(x-w/5, y-w, w, (roadYOffset/6) % 2 ? 0xbdf7 : 0xce59);
    display->drawFastVLine(x-w/5-1, y-w, w, (roadYOffset/6) % 2 ? 0xbdf7 : 0xce59);
    display->drawFastVLine(x-w/5-2, y-w, w, (roadYOffset/6) % 2 ? 0xbdf7 : 0xce59);
    display->drawFastVLine(x-w/5-3, y-w, w, (roadYOffset/6) % 2 ? 0xbdf7 : 0xce59);
    display->drawFastVLine(x+w+w/5, y-w, w, (roadYOffset/6) % 2 ? 0xbdf7 : 0xce59);
    display->drawFastVLine(x+w+w/5+1, y-w, w, (roadYOffset/6) % 2 ? 0xbdf7 : 0xce59);
    display->drawFastVLine(x+w+w/5+2, y-w, w, (roadYOffset/6) % 2 ? 0xbdf7 : 0xce59);
    display->drawFastVLine(x+w+w/5+3, y-w, w, (roadYOffset/6) % 2 ? 0xbdf7 : 0xce59);

    // Draw roof
    display->drawFastHLine(x-w/5, y-w, w+2*w/5, (roadYOffset/6) % 2 ? 0x5AEB : 0x8410);

    // Draw lamp light
    /*if (((roadYOffset/2) % 6) <= 2) {
      display->drawFastHLine(x + w/2 - w/4 -w/32, y-w+2, w/2 + w/16, 0xffed);
    }*/

    //Draw lamp
    if (((roadYOffset/2) % 6) == 1) {
      display->drawFastHLine(x + w/2 - w/4, y-w+1, w/2, 0xffff);
      display->drawFastHLine(x + w/2 - w/4, y-w+2, w/2, 0xffff);
      display->drawFastHLine(x + w/2 - w/4, y-w+3, w/2, 0xffff);
      display->drawFastHLine(x + w/2 - w/4, y-w+4, w/2, 0xffff);
    }

    // Draw enemy car
    drawEnemyCar(display, x, y, w, enemyY);
    drawEnemyCar(display, x, y, w, enemy2Y);
  }
  uint8_t playerTurnType = TURN_STRAIGHT;
  int8_t nitroEffectXOffset = 0;
  if (roadXOffset<-35) {
    playerTurnType = TURN_HARD_LEFT;
    nitroEffectXOffset = 10;
  } else if (roadXOffset<-5) {
    playerTurnType = TURN_LEFT;
    nitroEffectXOffset = 5;
  } else if (roadXOffset>5) {
    playerTurnType = TURN_RIGHT;
    nitroEffectXOffset = -5;
  } else if (roadXOffset>35) {
    playerTurnType = TURN_HARD_RIGHT;
    nitroEffectXOffset = -10;
  }
  playerCarSprite[playerTurnType*2 + ((blowData->ms/200) % 2)].pushToSprite(display, 125, 150, 0x0000);
  if (nitro>0 && animTime <= 2000 && !(blowData->lastBlowStatus & LAST_BLOW_FAILED)) {
    uint8_t animFrame = _max(0,_min(5,(2000-animTime)/400));
    nitroEffectSprite[animFrame].pushToSprite(display, 125 + nitroEffectXOffset, 170 + (animFrame <= 2 ? 6 : 0), 0x0000);
  }
}

void drawShortBlowGame_racing(DISPLAY_T* display, BlowData* blowData, String* errorMessage) {
  int32_t animTime = ((int32_t)blowData->ms) - ((int32_t)blowData->blowEndMs);
  for (uint8_t i = 0; i<=3; i++) {
    display->fillRect(i*80, 30, 80, 145, i%2 ? 0x5AEB : 0x8410);
  }
  for (uint8_t i = 0; i<7; i++) {
    display->fillRect(i*55, 95, 35, 10, 0xFFFF);
  }
  for (uint8_t i = 0; i<8; i++) {
    display->fillRect(i*40, 20, 40, 10, i%2 ? 0xF800 : 0xFFFF);
    display->fillRect(i*40, 165, 40, 10, i%2 ? 0xF800 : 0xFFFF);
  }
  if (animTime <= 2000 && !(blowData->lastBlowStatus & LAST_BLOW_FAILED)) {
    if (blowData->blowCount==1) {
      pitstopWheelSprite.pushToSprite(display, 15, 40 + animTime/40, 0x0000);
    }
    if (blowData->blowCount==2) {
      pitstopWheelSprite.pushToSprite(display, 85, 40 + animTime/40, 0x0000);
    }
    if (blowData->blowCount==3) {
      pitstopWheelSprite.pushToSprite(display, 15, 140 - animTime/40, 0x0000);
    }
    if (blowData->blowCount==4) {
      pitstopWheelSprite.pushToSprite(display, 85, 140 - animTime/40, 0x0000);
    }
    if (blowData->blowCount==5) {
      pitstopCarSprite[0].pushToSprite(display, animTime*animTime/5000, 70, 0x0000);
    }
  }
  if (blowData->blowCount<1) {
    pitstopWheelSprite.pushToSprite(display, 15, 40, 0x0000);
  }
  if (blowData->blowCount<2) {
    pitstopWheelSprite.pushToSprite(display, 85, 40, 0x0000);
  }
  if (blowData->blowCount<3) {
    pitstopWheelSprite.pushToSprite(display, 15, 140, 0x0000);
  }
  if (blowData->blowCount<4) {
    pitstopWheelSprite.pushToSprite(display, 85, 140, 0x0000);
  }
  if (blowData->blowCount<5) {
    pitstopCarSprite[0].pushToSprite(display, 0, 70, 0x0000);
  }
}

int8_t lastCheckedBlowCount = -1;
int8_t currentPosition = 31;
bool savedBlows=false;
unsigned long lastCheckedBlowEndMs = 0;
void drawLongBlowGame_racing(DISPLAY_T* display, BlowData* blowData, String* errorMessage) {
  int32_t animTime = ((int32_t)blowData->ms) - ((int32_t)blowData->blowEndMs);
  if (lastCheckedBlowCount==-1 || (blowData->blowCount>lastCheckedBlowCount && animTime>2000)) {
    currentTurnType = random(1,5);
    enemySpriteId = random(0,4);
    lastCheckedBlowCount = blowData->blowCount;
  }
  if (lastCheckedBlowEndMs < blowData->blowEndMs) {
    if (blowData -> lastBlowStatus == LAST_BLOW_FAILED) {
      currentPosition++;
    } else {
      currentPosition--;
      if (nitro>0) {
        nitro--;
        currentPosition--;
      }
    }
    lastCheckedBlowEndMs = blowData->blowEndMs;
  }

  
  drawRace(display, blowData, animTime);
  display->setTextSize(4);
  display->setCursor(260, 200);
  if (currentPosition<10) {
    display->print(" ");
  }
  display->print(currentPosition);

  if (nitro > 0) {
    display->setCursor(265, 12);
    display->setTextSize(3);
    printShaded(display, String(nitro) + "x");
    nitroSSprite.pushToSprite(display, 300, 5, 0x0000);
  }

  if (blowData->cycleNumber==CYCLES-1 && blowData->blowCount==LONG_BLOW_NUMBER_MAX-1 && !savedBlows) {
    savedBlows = true;
    prefs.putUInt("nitro", nitro);
    Serial.print("Saved ");
    Serial.print(nitro);
    Serial.println(" nitro.");
  }
}

bool savedTrampoline=false;
void drawTrampolineGame_racing(DISPLAY_T* display, JumpData* jumpData, String* errorMessage) {
  uint8_t earnedNitro = (jumpData->jumpCount / 100);
  uint8_t currentBottle = 0;
  int32_t bottleSpacingX = (nitroLSprite.width()+5);
  for (uint8_t currentBottle=0; currentBottle<earnedNitro; currentBottle++) {
    nitroLSprite.pushToSprite(display, 50 + bottleSpacingX*currentBottle, 70, 0x0000);
  }
  nitroLSprite.pushToSprite(display, 50 + bottleSpacingX*(earnedNitro), 70, 0x0000);
  display->fillRect(50+bottleSpacingX*(earnedNitro), 70, nitroLSprite.width(), nitroLSprite.height()*(100-(jumpData->jumpCount%100))/100, 0x0000);
  if (!savedTrampoline && jumpData->msLeft<0) {
    savedTrampoline = true;
    nitro += earnedNitro;
    prefs.putUInt("nitro", nitro);
    Serial.print("Saved ");
    Serial.print(nitro);
    Serial.println(" nitro.");
  } 
}