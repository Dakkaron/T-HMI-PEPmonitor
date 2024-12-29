#include <Arduino.h>
#define CONFIG_SPIRAM_SUPPORT 1

#include "systemconfig.h"
#include "constants.h"

#include "pins.h"

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

void runGameSelection() {
  String errorMessage;
  uint16_t numberOfGames = getNumberOfGames(&errorMessage);
  checkFailWithMessage(errorMessage);
  Serial.print("Number of games: ");
  Serial.println(numberOfGames);
  String gamePath;
  if (numberOfGames == 1) {
    gamePath = getGamePath(0, &errorMessage);
  } else {
    gamePath = getGamePath(displayGameSelection(&spr, numberOfGames, &errorMessage), &errorMessage);
  }
  checkFailWithMessage(errorMessage);
  Serial.print("Game path: ");
  Serial.println(gamePath);
  initGames(gamePath, &errorMessage);
  checkFailWithMessage(errorMessage);
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

  // Initialize the HX711
  hx711.begin();

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

  // Tare 3x
  Serial.print(F("Tareing air pressure sensor...."));
  for (uint8_t t=0; t<3; t++) {
    hx711.tareA(hx711.readChannelRaw(CHAN_A_GAIN_64));
    hx711.tareA(hx711.readChannelRaw(CHAN_A_GAIN_64));
  }
  Serial.print(F("done"));

  Serial.println(F("Initializing SD..."));
  SD_MMC.setPins(SD_SCLK_PIN, SD_MOSI_PIN, SD_MISO_PIN);
  bool rlst = SD_MMC.begin("/sdcard", true);
  if (!rlst) {
    Serial.println("SD init failed");
    Serial.println("? No detected SdCard");
    checkFailWithMessage("SD Karte nicht lesbar!");
  } else {
    Serial.println("SD init success");
    Serial.printf("? Detected SdCard insert: %.2f GB\r\n", SD_MMC.cardSize() / 1024.0 / 1024.0 / 1024.0);
  }
  Serial.println(F("done"));
  String errorMessage;
  initSystemConfig(&errorMessage);
  checkFailWithMessage(errorMessage);
  Serial.print("PEPit Version '");
  Serial.print(VERSION);
  Serial.println("' initialized");
  runProfileSelection(&errorMessage);
  spr.fillSprite(TFT_BLACK);
  spr.pushSpriteFast(0,0);
  spr.fillSprite(TFT_BLACK);
  checkFailWithMessage(errorMessage);
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