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
#include "pressuresensor.h"
#include "touchHandler.h"
#include "wifiHandler.h"
#include "sdHandler.h"
#include "games.h"
#include "serialHandler.h"

#include <OneButton.h>

Adafruit_HX711 hx711(HX7711_DATA_PIN, HX7711_CLOCK_PIN);
BlowData blowData;
JumpData jumpData;

uint8_t sensorMode = SENSOR_MODE_PEPS;

OneButton buttonPwr(BUTTON2_PIN, false, false);
OneButton buttonUsr(BUTTON1_PIN, false, false);

TFT_eSprite batteryIcon[] = {TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft)};

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
        spr.pushSpriteFast(0,0);
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
        spr.pushSpriteFast(0,0);
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

void checkFailWithMessage(String message) {
  if (!message.isEmpty()) {
    tft.fillScreen(TFT_BLACK);
    spr.fillSprite(TFT_BLACK);
    spr.setCursor(1, 16);
    spr.println("FEHLER:");
    spr.println(message);
    spr.pushSprite(0, 0);
    while (true) {};
  }
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

  Serial.begin(115200);
  delay(1000);

  pinMode(PWR_EN_PIN, OUTPUT);
  digitalWrite(PWR_EN_PIN, HIGH);

  Serial.print("PEPit Version '");
  Serial.print(VERSION);
  Serial.println("'");
  Serial.print("Initializing peripherals....");
  delay(1000);

  // Initialize the HX711
  hx711.begin();
  
  spr.setColorDepth(8);
  spr.createSprite(SCREEN_WIDTH, SCREEN_HEIGHT, 2);
  
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
  spr.pushSpriteFast(0, 0);

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
    checkFailWithMessage("SD Karte nicht lesbar!");
  } else {
    Serial.println("SD init success");
    Serial.printf("? Detected SdCard insert: %.2f GB\r\n", SD_MMC.cardSize() / 1024.0 / 1024.0 / 1024.0);
  }
  Serial.println(F("done"));
  Serial.print("PEPit Version '");
  Serial.print(VERSION);
  Serial.println("' initialized");

  loadBmp(&batteryIcon[0], "/gfx/battery_low.bmp");
  loadBmp(&batteryIcon[1], "/gfx/battery_half.bmp");
  loadBmp(&batteryIcon[2], "/gfx/battery_full.bmp");

  runGameSelection();
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
// Draws battery icon, battery voltage, FPS
void drawSystemStats() {
  static uint32_t lowBatteryCount = 0;
  uint32_t batteryVoltage = readBatteryVoltage();
  if (batteryVoltage < 3500) {
    batteryIcon[0].pushToSprite(&spr, 1, 1, 0x0000);
  } else if (batteryVoltage < 3800) {
    batteryIcon[1].pushToSprite(&spr, 1, 1, 0x0000);
  } else if (batteryVoltage < 4100) {
    batteryIcon[2].pushToSprite(&spr, 1, 1, 0x0000);
  } else if (batteryVoltage < 4400) {
    batteryIcon[0].pushToSprite(&spr, 1, 1, 0x0000);
  } else if (batteryVoltage < 4600) {
    batteryIcon[1].pushToSprite(&spr, 1, 1, 0x0000);
  } else {
    batteryIcon[2].pushToSprite(&spr, 1, 1, 0x0000);
  }
  if (batteryVoltage<BATTERY_LOW_WARNING_VOLTAGE && (blowData.ms/1000)%2) {
    batteryIcon[0].pushToSprite(&spr, 144, 110, 0x0000);
  }
  if (batteryVoltage<BATTERY_LOW_SHUTDOWN_VOLTAGE) {
    lowBatteryCount++;
  } else {
    lowBatteryCount = 0;
  }
  if (lowBatteryCount>100) {
    power_off();
  }
  spr.setTextSize(1);
  spr.setCursor(34,1);
  spr.print(String(1000L/_max(1,blowData.ms-lastMs))); //FPS counter
  spr.setCursor(34,11);
  spr.print(String(batteryVoltage/1000) + "." + leftPad(String(batteryVoltage%1000), 3, "0") + "V"); // Battery voltage
}

void drawPEPDisplay() {
  spr.fillSprite(TFT_BLACK);
  String errorMessage;
  if (blowData.isLongBlows) {
    drawLongBlowGame(&spr, &blowData, &errorMessage);
    drawProgressBar(&spr, blowData.currentlyBlowing ? (100 * (blowData.ms - blowData.blowStartMs) / LONG_BLOW_DURATION_MS) : 0, 0, PRESSURE_BAR_X, PRESSURE_BAR_Y+25, PRESSURE_BAR_WIDTH, PRESSURE_BAR_HEIGHT);
  } else {
    drawShortBlowGame(&spr, &blowData, &errorMessage);
    drawProgressBar(&spr, blowData.currentlyBlowing ? (100 * (blowData.ms - blowData.blowStartMs) / SHORT_BLOW_DURATION_DISPLAY_MS) : 0, 0, PRESSURE_BAR_X, PRESSURE_BAR_Y+25, PRESSURE_BAR_WIDTH, PRESSURE_BAR_HEIGHT);
  }
  checkFailWithMessage(errorMessage);
  
  drawProgressBar(&spr, blowData.pressure, 10, PRESSURE_BAR_X, PRESSURE_BAR_Y, PRESSURE_BAR_WIDTH, PRESSURE_BAR_HEIGHT);
  spr.setCursor(PRESSURE_BAR_X + 20, PRESSURE_BAR_Y - 14);
  spr.setTextSize(2);
  printShaded(&spr, String(blowData.blowCount) + "/" + String(blowData.cycleNumber));
  drawSystemStats();
  spr.pushSpriteFast(0, 0);
}

String winScreenPath = "";
void drawFinished() {
  if (winScreenPath == "") {
    String errorMessage;
    winScreenPath = getRandomWinScreenPathForCurrentGame(&errorMessage);
    checkFailWithMessage(errorMessage);
  }
  drawBmp(winScreenPath, 0, 0);
  drawSystemStats();
}

void drawTrampolineDisplay() {
  spr.fillSprite(TFT_BLACK);
  String errorMessage;
  drawTrampolineGame(&spr, &jumpData, &errorMessage);
  checkFailWithMessage(errorMessage);
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
  drawSystemStats();
  spr.pushSpriteFast(0, 0);
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
    spr.pushSpriteFast(0, 0);
    delay(200);
  }
  tft.invertDisplay(0);
}

uint32_t showDisplayRotate = 0;
void loop() {
  lastMs = blowData.ms;
  blowData.ms = millis();
  jumpData.ms = blowData.ms;


  buttonPwr.tick();
  buttonUsr.tick();
  handleSerial();
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
      blowData.blowStartMs = blowData.ms;
      blowData.maxPressure = 0;
      blowData.cumulativeError = 0;
      blowData.lastBlowStatus |= NEW_BLOW;
    } else if (blowData.pressure<=PRESSURE_SENSOR_BLOWING_THRESHOLD && blowData.currentlyBlowing) {
      blowData.blowEndMs = blowData.ms;
      Serial.print(blowData.blowEndMs-blowData.blowStartMs);
      Serial.println(F("ms"));
      blowData.currentlyBlowing = false;
      if (blowData.isLongBlows) { // long blows
        Serial.print(F("Ending long blow"));
        if (blowData.ms-blowData.blowStartMs > LONG_BLOW_DURATION_MS) {
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