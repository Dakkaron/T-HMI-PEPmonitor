#include <Arduino.h>
#define CONFIG_SPIRAM_SUPPORT 1

#include "systemconfig.h"
#include "constants.h"

#include <SD_MMC.h>
#include <FS.h>
#include <SPI.h>
#include <SD.h>
#include <OneButton.h>

#include "hardware/gfxHandler.hpp"
#include "hardware/pressuresensor.h"
#include "hardware/touchHandler.h"
#include "hardware/wifiHandler.h"
#include "hardware/sdHandler.h"
#include "games/games.h"
#include "hardware/serialHandler.h"
#include "hardware/powerHandler.h"
#include "physioProtocolHandler.h"

void setBrightness(uint8_t value) {
  static uint8_t _brightness = 0;

  if (_brightness == value) {
    return;
  }
  value = _min(16, value);
  if (value == 0) {
    digitalWrite(BK_LIGHT_PIN, 0);
    delay(3);
    _brightness = 0;
    return;
  }
  if (_brightness == 0) {
    digitalWrite(BK_LIGHT_PIN, 1);
    _brightness = SCREEN_BRIGHTNESS_STEPS;
    delayMicroseconds(30);
  }
  int from = SCREEN_BRIGHTNESS_STEPS - _brightness;
  int to = SCREEN_BRIGHTNESS_STEPS - value;
  int num = (SCREEN_BRIGHTNESS_STEPS + to - from) % SCREEN_BRIGHTNESS_STEPS;
  for (int i = 0; i < num; i++) {
    digitalWrite(BK_LIGHT_PIN, 0);
    digitalWrite(BK_LIGHT_PIN, 1);
  }
  _brightness = value;
}

void setup() {
  pinMode(PWR_ON_PIN, OUTPUT);
  digitalWrite(PWR_ON_PIN, HIGH);
  pinMode(BK_LIGHT_PIN, OUTPUT);
  digitalWrite(BK_LIGHT_PIN, HIGH);

  Serial.setRxBufferSize(10240);
  Serial.setTxBufferSize(1024);
  Serial.begin(115200);

  pinMode(PWR_EN_PIN, OUTPUT);
  digitalWrite(PWR_EN_PIN, HIGH);

  tft.init();
  tft.setRotation(SCREEN_ROTATION);
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_BLACK);

  setBrightness(16);

  spr.setColorDepth(8);
  spr.createSprite(SCREEN_WIDTH, SCREEN_HEIGHT, 2);
  for (uint32_t i=0;i<2;i++) {
    spr.fillSprite(TFT_BLACK);
    spr.setTextSize(2);
    spr.setTextColor(TFT_WHITE);
    spr.setCursor(1, 16);
    spr.println("Nicht");
    spr.println("blasen!");
    spr.pushSpriteFast(0, 0);
  }
  spr.pushSprite(0, 0);
  initTouch();

  buttonPwr.attachClick(power_off);
  buttonPwr.attachLongPressStop(power_off);
  buttonUsr.attachLongPressStop(runTouchCalibration);

  randomSeed(analogRead(0));
  
  delay(1000);
  Serial.print("PEPit Version '");
  Serial.print(VERSION);
  Serial.println("'");
  Serial.print("Initializing peripherals....");
  delay(1000);

  Serial.println(F("done"));

  initPressureSensor();
  
  String errorMessage;
  initSD(&errorMessage);
  checkFailWithMessage(errorMessage);
  initSystemConfig(&errorMessage);
  checkFailWithMessage(errorMessage);
  
  Serial.print("PEPit Version '");
  Serial.print(VERSION);
  Serial.println("' initialized");

  runProfileSelection();
  runGameSelection();
  displayPhysioRotateScreen();
}

void loop() {
  buttonPwr.tick();
  buttonUsr.tick();
  handleSerial();
  handlePhysioTask();
  /*Serial.print("Free heap: ");
  Serial.println(ESP.getFreeHeap());*/
}