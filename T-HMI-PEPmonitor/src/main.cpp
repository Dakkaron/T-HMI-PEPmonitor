#include <Arduino.h>
#define CONFIG_SPIRAM_SUPPORT 1

#include "config.h"
#include "constants.h"

#include "pins.h"

#include <SD_MMC.h>
#include <FS.h>
#include <SPI.h>
#include <SD.h>

#include "gfxHandler.hpp"
#include "games.hpp"
#include "pressuresensor.h"
#include "touchHandler.h"
#include "wifiHandler.h"
#include "sdHandler.h"

#include <OneButton.h>

Adafruit_HX711 hx711(HX7711_DATA_PIN, HX7711_CLOCK_PIN);
BlowData blowData;
JumpData jumpData;

uint8_t sensorMode = SENSOR_MODE_PEPS;

OneButton buttonPwr(BUTTON2_PIN, false, false);
OneButton buttonUsr(BUTTON1_PIN, false, false);


void power_off() {
    digitalWrite(PWR_ON_PIN, LOW);
    Serial.println("power_off");
}


uint8_t trampolineConnectionStatus = CONNECTION_NOTCONNECTED;
void switchMode() {
  Serial.print("-> CONN STAT: ");
  Serial.println(trampolineConnectionStatus);
  Serial.print("Switching mode to ");
  switch (sensorMode) {
    case SENSOR_MODE_PEPS:
      if (trampolineConnectionStatus != CONNECTION_OK) {
        spr.fillSprite(TFT_BLACK);
        spr.setTextSize(2);
        spr.setTextColor(TFT_WHITE);
        spr.setCursor(1, 16);
        spr.print("Verbinde...");
        spr.pushSprite(0,0);
        trampolineConnectionStatus = connectToTrampoline();
        Serial.print("-1> CONN STAT: ");
        Serial.println(trampolineConnectionStatus);
      }
      if (trampolineConnectionStatus != CONNECTION_OK) {
        Serial.println("Trampoline failed: no connection");
        Serial.println("Not changing mode");
        spr.fillSprite(TFT_BLACK);
        spr.setTextSize(2);
        spr.setTextColor(TFT_WHITE);
        spr.setCursor(1, 16);
        spr.print("Keine Verbindung!");
        spr.pushSprite(0,0);
        delay(5000);
        break;
      }
      sensorMode = SENSOR_MODE_TRAMPOLINE;
      Serial.println("Trampoline");
      break;
    case SENSOR_MODE_TRAMPOLINE:
      sensorMode = SENSOR_MODE_PEPS;
      Serial.println("PEP S");
      break;
    default:
      sensorMode = SENSOR_MODE_PEPS;
      Serial.println("PEP S");
      break;
  }
}

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

  Serial.begin(115200);
  delay(1000);

  pinMode(PWR_EN_PIN, OUTPUT);
  digitalWrite(PWR_EN_PIN, HIGH);

  Serial.print("Initializing peripherals....");
  delay(1000);

  // Initialize the HX711
  hx711.begin();
  
  spr.createSprite(SCREEN_WIDTH, SCREEN_HEIGHT);
  spr.setColorDepth(8); //redundant

  initTouch();
  tft.init();
  tft.setRotation(SCREEN_ROTATION);
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_BLACK);

  setBrightness(16);

  spr.fillSprite(TFT_BLACK);
  spr.setTextSize(2);
  spr.setTextColor(TFT_WHITE);
  spr.setCursor(1, 16);
  spr.println("Nicht");
  spr.println("blasen!");
  spr.pushSprite(0, 0);

  Serial.println(F("done"));

  randomSeed(analogRead(0));

  buttonPwr.attachClick(power_off);
  buttonPwr.attachLongPressStop(power_off);
  buttonUsr.attachClick(switchMode);
  buttonUsr.attachLongPressStop(switchMode);

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
  } else {
      Serial.println("SD init success");
      Serial.printf("? Detected SdCard insert: %.2f GB\r\n", SD_MMC.cardSize() / 1024.0 / 1024.0 / 1024.0);
  }

  initGames();
  Serial.println(F("done"));
}

uint32_t readBatteryVoltage() {
    return (analogRead(BAT_ADC_PIN) * 162505) / 100000;
}

String leftPad(String s, uint16_t len, String c) {
  while (s.length()<len) {
    s = c + s;
  }
  return s;
}

uint32_t lastMs = 0;
void drawPEPDisplay() {
  spr.fillSprite(TFT_BLACK);
  blowData.ms = millis();
  if (blowData.isLongBlows) {
    drawLongBlowGame(blowData.cycleNumber, &spr, &blowData);
    drawProgressBar(&spr, blowData.currentlyBlowing ? (100 * (blowData.ms - blowData.blowStartMs) / LONG_BLOW_DURATION_MS) : 0, 0, PRESSURE_BAR_X, PRESSURE_BAR_Y+25, PRESSURE_BAR_WIDTH, PRESSURE_BAR_HEIGHT);
  } else {
    drawShortBlowGame(blowData.cycleNumber, &spr, &blowData);
    drawProgressBar(&spr, blowData.currentlyBlowing ? (100 * (blowData.ms - blowData.blowStartMs) / SHORT_BLOW_DURATION_DISPLAY_MS) : 0, 0, PRESSURE_BAR_X, PRESSURE_BAR_Y+25, PRESSURE_BAR_WIDTH, PRESSURE_BAR_HEIGHT);
  }
  
  drawProgressBar(&spr, blowData.pressure, 10, PRESSURE_BAR_X, PRESSURE_BAR_Y, PRESSURE_BAR_WIDTH, PRESSURE_BAR_HEIGHT);
  spr.setCursor(PRESSURE_BAR_X + 20, PRESSURE_BAR_Y - 14);
  spr.setTextSize(2);
  spr.print(blowData.blowCount);
  spr.print("/");
  spr.print(blowData.cycleNumber);
  spr.setTextSize(1);
  spr.setCursor(1,1);
  spr.println(1000L/_max(1,millis()-lastMs)); //FPS counter
  uint32_t batteryVoltage = readBatteryVoltage();
  spr.print(batteryVoltage/1000); // Battery voltage
  spr.print(".");
  spr.print(leftPad(String(batteryVoltage%1000), 3, "0"));
  spr.println("V");
  lastMs = millis();
  spr.pushSprite(0, 0);
}

String winScreenPath = "";
void drawFinished() {
  if (winScreenPath == "") {
    winScreenPath = getRandomWinScreenPath();
  }
  drawBmp(winScreenPath, 0, 0);
}

void drawTrampolineDisplay() {
  spr.fillSprite(TFT_BLACK);
  drawTrampolineGame(0, &spr, &jumpData);
  if (jumpData.msLeft > 0) {
    drawProgressBar(&spr, (100L*(jumpData.totalTime-jumpData.msLeft))/jumpData.totalTime, 0, PRESSURE_BAR_X, PRESSURE_BAR_Y, PRESSURE_BAR_WIDTH, PRESSURE_BAR_HEIGHT);
    spr.setCursor(PRESSURE_BAR_X + 20, PRESSURE_BAR_Y - 14);
    spr.setTextSize(2);
    spr.print(jumpData.jumpCount);
    spr.print("/");
    spr.print(jumpData.highscore);
    spr.setCursor(100, 5);
    spr.setTextSize(3);
    int32_t secondsLeft = _max(0, jumpData.msLeft/1000);
    spr.print(secondsLeft / 60);
    spr.print(":");
    if ((secondsLeft % 60) < 10) {
      spr.print("0");
    }
    spr.print(secondsLeft % 60);
  }
  spr.setTextSize(1);
  spr.setCursor(1,1);
  spr.println(1000L/_max(1,millis()-lastMs)); //FPS counter
  uint32_t batteryVoltage = readBatteryVoltage();
  spr.print(batteryVoltage/1000); // Battery voltage
  spr.print(".");
  spr.print(leftPad(String(batteryVoltage%1000), 3, "0"));
  spr.print("V");
  lastMs = millis();
  spr.pushSprite(0, 0);
}



void displayRotatePEP() {
  for (uint8_t i=0;i<25;i++) {
    tft.invertDisplay(i%2);
    spr.fillSprite(TFT_BLACK);
    spr.setTextSize(5);
    spr.setTextColor(TFT_WHITE);
    spr.setCursor(20, 20);
    if (blowData.isLongBlows) {
      spr.print("2.5mm");
    } else {
      spr.print("5mm");
    }
    spr.pushSprite(0, 0);
    delay(200);
  }
  tft.invertDisplay(0);
}

uint32_t showDisplayRotate = 0;
void loop() {
  buttonPwr.tick();
  buttonUsr.tick();
  if (sensorMode == SENSOR_MODE_PEPS) {
    if (blowData.cycleNumber >= CYCLES) {
      drawFinished();
      return;
    }
    spr.invertDisplay(0);
    readPressure(&hx711, &blowData);
    if (blowData.pressure>PRESSURE_SENSOR_BLOWING_THRESHOLD && !blowData.currentlyBlowing) {
      Serial.print(F("Blowing... "));
      blowData.currentlyBlowing = true;
      blowData.blowStartMs = millis();
      blowData.maxPressure = 0;
      blowData.cumulativeError = 0;
      blowData.lastBlowStatus |= NEW_BLOW;
    } else if (blowData.pressure<=PRESSURE_SENSOR_BLOWING_THRESHOLD && blowData.currentlyBlowing) {
      blowData.blowEndMs = millis();
      Serial.print(blowData.blowEndMs-blowData.blowStartMs);
      Serial.println(F("ms"));
      blowData.currentlyBlowing = false;
      if (blowData.isLongBlows) { // long blows
        Serial.print(F("Ending long blow"));
        if (millis()-blowData.blowStartMs > LONG_BLOW_DURATION_MS) {
          blowData.blowCount++;
          blowData.lastBlowStatus = LAST_BLOW_SUCCEEDED;
          Serial.println(F(" successfully"));
          if (blowData.blowCount >= LONG_BLOW_NUMBER_MAX) {
            showDisplayRotate  = blowData.ms + 2000;
          }
        } else {
          blowData.fails++;
          blowData.lastBlowStatus = LAST_BLOW_FAILED;
        }
      } else { // short blows
        if (blowData.maxPressure > SHORT_BLOW_MIN_STRENGTH) {
          blowData.blowCount++;
          blowData.lastBlowStatus = LAST_BLOW_SUCCEEDED;
          if (blowData.blowCount >= SHORT_BLOW_NUMBER_MAX) {
            showDisplayRotate = blowData.ms + 2000;
          }
        } else {
          blowData.fails++;
          blowData.lastBlowStatus = LAST_BLOW_FAILED;
        }
      }
    }
    blowData.maxPressure = _max(blowData.maxPressure, blowData.pressure);
    if (showDisplayRotate != 0 && blowData.ms > showDisplayRotate) {
      Serial.println();
      Serial.println("##### DISPLAY ROTATE ######");
      Serial.print("    Rotating to: ");
      Serial.println(!blowData.isLongBlows ? "LONG BLOWS": "SHORT BLOWS");
      Serial.println();
      blowData.blowCount = 0;
      blowData.cycleNumber += !blowData.isLongBlows;
      blowData.isLongBlows = !blowData.isLongBlows;
      blowData.lastBlowStatus = 0;
      showDisplayRotate = 0;
      if (blowData.cycleNumber < CYCLES) {
        displayRotatePEP();
      }
    }
    drawPEPDisplay();
  } else if (sensorMode == SENSOR_MODE_TRAMPOLINE) {
    if (jumpData.msLeft < -5000) {
      drawFinished();
      return;
    }
    getJumpData(&jumpData);
    drawTrampolineDisplay();
  }
  /*Serial.print("Free heap: ");
  Serial.println(ESP.getFreeHeap());*/
}