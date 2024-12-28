#include "gfxHandler.hpp"
#include <SD_MMC.h>
#include <FS.h>
#include <SPI.h>
#include <SD.h>
#include <algorithm>
#include "hardware/touchHandler.h"
#include "hardware/sdHandler.h"
#include "hardware/serialHandler.h"
#include "hardware/powerHandler.h"
#include "config.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);
TFT_eSprite batteryIcon[] = {TFT_eSprite(&tft), TFT_eSprite(&tft), TFT_eSprite(&tft)};

uint16_t read16(fs::File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}


uint32_t read32(fs::File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

void loadBmp(DISPLAY_T* display, String filename) {
  loadBmp(display, filename, 0);
}

void loadBmp(DISPLAY_T* display, String filename, uint8_t flipped) {
  loadBmpAnim(&display, filename, 1, flipped);
}

void loadBmpAnim(DISPLAY_T** display, String filename, uint8_t animFrames) {
  loadBmpAnim(display, filename, animFrames, 0);
}

void getBmpDimensions(String filename, int16_t* w, int16_t* h) {
  File bmpFS;
  bmpFS = SD_MMC.open(filename);

  if (!bmpFS)
  {
    Serial.print("File not found: ");
    Serial.println(filename);
    Serial.println("#");
    return;
  }
  uint16_t headerBytes = read16(bmpFS);
  if (headerBytes == 0x4D42) {
    read32(bmpFS);
    read32(bmpFS);
    read32(bmpFS);
    read32(bmpFS);
    *w = read32(bmpFS);
    *h = read32(bmpFS);
  } else {
    Serial.print("Wrong file format: ");
    Serial.println(headerBytes);
  }
  bmpFS.close();
}

/*
 * animFrames -> number of frames to export, 1 == no animation, still image
 */
void loadBmpAnim(DISPLAY_T** displays, String filename, uint8_t animFrames, uint8_t flipped) {
  Serial.print("File: ");
  Serial.println(filename);
  Serial.println("#");

  File bmpFS;

  // Open requested file on SD card
  bmpFS = SD_MMC.open(filename);

  if (!bmpFS)
  {
    Serial.print("File not found: ");
    Serial.println(filename);
    Serial.println("#");
    return;
  }

  uint32_t seekOffset;
  uint16_t w, h, row, col, frameNr, frameH;
  uint8_t  r, g, b, a;

  uint32_t startTime = millis();

  uint16_t headerBytes = read16(bmpFS);
  if (headerBytes == 0x4D42) {
    read32(bmpFS);
    read32(bmpFS);
    seekOffset = read32(bmpFS);
    read32(bmpFS);
    w = read32(bmpFS);
    h = read32(bmpFS);
    frameH = h / animFrames;

    /*for (uint8_t i=0; i < animFrames; i++) {
      if ((w != displays[i]->width()) || (frameH != displays[i]->height())) {
        displays[i]->deleteSprite();
        displays[i]->createSprite(w, frameH);
      }
    }*/

    uint16_t colorPanes = read16(bmpFS);
    uint16_t bitDepth = read16(bmpFS);
    uint16_t compression = read16(bmpFS);

    if ((colorPanes == 1) && (((bitDepth == 24) && (compression == 0)) || ((bitDepth == 32) && (compression == 3)))) { // BITMAPINFOHEADER
      uint8_t bytesPerPixel = bitDepth/8;
      
      for (uint8_t i=0; i < animFrames; i++) {
        displays[i]->setSwapBytes(true);
        displays[i]->fillSprite(TFT_BLACK);
      }
      bmpFS.seek(seekOffset);

      uint16_t padding;
      if (bitDepth == 24) {
        padding = (4 - ((w * 3) & 3)) & 3;
      } else if (bitDepth == 32) {
        padding = 0;
      }
      uint8_t lineBuffer[w * bytesPerPixel];

      for (frameNr = 0; frameNr < animFrames; frameNr++) {
        if (displays[frameNr]->created() && (displays[frameNr]->width()!=w || displays[frameNr]->height()!=frameH)) {
          displays[frameNr]->deleteSprite();
        }
        if (!displays[frameNr]->created()) {
          displays[frameNr]->createSprite(w, frameH);
        }

        Serial.print("Loading frame: ");
        Serial.println(frameNr);
        for (row = 0; row < frameH; row++) {
          bmpFS.read(lineBuffer, sizeof(lineBuffer));
          uint8_t*  bptr = lineBuffer;
          uint16_t* tptr = (uint16_t*)lineBuffer;
          // Convert 24 to 16 bit colours
          for (uint16_t col = 0; col < w; col++) {
            b = *bptr++;
            g = *bptr++;
            r = *bptr++;
            if (bytesPerPixel == 4) {
              a = *bptr++;
              if (a == 0) {
                *tptr++ = 0x0000;
              } else {
                uint16_t res = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
                if (res == 0x0000) {
                  res = 0x0001;
                }
                *tptr++ = res;
              }
            } else {
                *tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
            }
          }
          // Read any line padding
          if (padding) {
            bmpFS.read((uint8_t*)tptr, padding);
          }

          if (flipped & FLIPPED_H) {
            std::reverse((uint16_t*)lineBuffer, ((uint16_t*)lineBuffer)+w);
          }
          // Push the pixel row to screen, pushImage will crop the line if needed
          if (flipped & FLIPPED_V) {
            displays[frameNr]->pushImage(0, row, w, 1, (uint16_t*)lineBuffer);
          } else {
            displays[frameNr]->pushImage(0, frameH - 1 - row, w, 1, (uint16_t*)lineBuffer);
          }
        }
      }
      Serial.print("Loaded in "); Serial.print(millis() - startTime);
      Serial.println(" ms");
    } else {
      Serial.print("BMP format not recognized: ");
      Serial.print(colorPanes);
      Serial.print(" ");
      Serial.print(bitDepth);
      Serial.print(" ");
      Serial.println(compression);
    }
  } else {
    Serial.print("Wrong file format: ");
    Serial.println(headerBytes);
  }
  bmpFS.close();
}

void drawBmp(String filename, int16_t x, int16_t y, bool debugLog) {
  drawBmpSlice(filename, x, y, -1, debugLog);
}

void drawBmpSlice(String filename, int16_t x, int16_t y, int16_t maxH, bool debugLog) {
  if (debugLog) {
    Serial.print("File: ");
    Serial.println(filename);
    Serial.println("#");
  }

  File bmpFS;

  // Open requested file on SD card
  bmpFS = SD_MMC.open(filename);

  if (!bmpFS)
  {
    Serial.print("File not found: ");
    Serial.println(filename);
    Serial.println("#");
    return;
  }

  uint32_t seekOffset;
  uint16_t w, h, row, col, frameNr;
  uint8_t  r, g, b, a;

  uint32_t startTime = millis();

  uint16_t headerBytes = read16(bmpFS);
  if (headerBytes == 0x4D42) {
    read32(bmpFS);
    read32(bmpFS);
    seekOffset = read32(bmpFS);
    read32(bmpFS);
    w = read32(bmpFS);
    h = read32(bmpFS);

    uint16_t colorPanes = read16(bmpFS);
    uint16_t bitDepth = read16(bmpFS);
    uint16_t compression = read16(bmpFS);

    if ((colorPanes == 1) && (((bitDepth == 24) && (compression == 0)) || ((bitDepth == 32) && (compression == 3)))) { // BITMAPINFOHEADER
      uint8_t bytesPerPixel = bitDepth/8;
      
      tft.setSwapBytes(true);
      maxH = maxH == -1 ? h : maxH;
      bmpFS.seek(seekOffset + (h-maxH)*w*bytesPerPixel);

      uint16_t padding;
      if (bitDepth == 24) {
        padding = (4 - ((w * 3) & 3)) & 3;
      } else if (bitDepth == 32) {
        padding = 0;
      }
      uint8_t lineBuffer[w * bytesPerPixel];

      for (row = h-maxH; row < h; row++) {
        bmpFS.read(lineBuffer, sizeof(lineBuffer));
        uint8_t*  bptr = lineBuffer;
        uint16_t* tptr = (uint16_t*)lineBuffer;
        // Convert 24 to 16 bit colours
        for (uint16_t col = 0; col < w; col++) {
          b = *bptr++;
          g = *bptr++;
          r = *bptr++;
          if (bytesPerPixel == 4) {
            a = *bptr++;
            if (a == 0) {
              *tptr++ = 0x0000;
            } else {
              uint16_t res = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
              if (res == 0x0000) {
                res = 0x0001;
              }
              *tptr++ = res;
            }
          } else {
            *tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
          }
        }
        // Read any line padding
        if (padding) {
          bmpFS.read((uint8_t*)tptr, padding);
        }
        // Push the pixel row to screen, pushImage will crop the line if needed
        tft.pushImage(x, y + h - 1 - row, w, 1, (uint16_t*)lineBuffer, 0x0000);
      }
      if (debugLog) {
        Serial.print("Loaded in "); Serial.print(millis() - startTime);
        Serial.println(" ms");
      }
    } else {
      Serial.print("BMP format not recognized: ");
      Serial.print(colorPanes);
      Serial.print(" ");
      Serial.print(bitDepth);
      Serial.print(" ");
      Serial.println(compression);
    }
  } else {
    Serial.print("Wrong file format: ");
    Serial.println(headerBytes);
  }
  bmpFS.close();
}

void drawBmp(DISPLAY_T* sprite, String filename, int16_t x, int16_t y, bool debugLog) {
  if (debugLog) {
    Serial.print("File: ");
    Serial.println(filename);
    Serial.println("#");
  }

  File bmpFS;

  // Open requested file on SD card
  bmpFS = SD_MMC.open(filename);

  if (!bmpFS)
  {
    Serial.print("File not found: ");
    Serial.println(filename);
    Serial.println("#");
    return;
  }

  uint32_t seekOffset;
  uint16_t w, h, row, col, frameNr;
  uint8_t  r, g, b, a;

  uint32_t startTime = millis();

  uint16_t headerBytes = read16(bmpFS);
  if (headerBytes == 0x4D42) {
    read32(bmpFS);
    read32(bmpFS);
    seekOffset = read32(bmpFS);
    read32(bmpFS);
    w = read32(bmpFS);
    h = read32(bmpFS);

    uint16_t colorPanes = read16(bmpFS);
    uint16_t bitDepth = read16(bmpFS);
    uint16_t compression = read16(bmpFS);

    if ((colorPanes == 1) && (((bitDepth == 24) && (compression == 0)) || ((bitDepth == 32) && (compression == 3)))) { // BITMAPINFOHEADER
      uint8_t bytesPerPixel = bitDepth/8;
      
      sprite->setSwapBytes(true);
      bmpFS.seek(seekOffset);

      uint16_t padding;
      if (bitDepth == 24) {
        padding = (4 - ((w * 3) & 3)) & 3;
      } else if (bitDepth == 32) {
        padding = 0;
      }
      uint8_t lineBuffer[w * bytesPerPixel];

      for (row = 0; row < h; row++) {
        bmpFS.read(lineBuffer, sizeof(lineBuffer));
        uint8_t*  bptr = lineBuffer;
        uint16_t* tptr = (uint16_t*)lineBuffer;
        // Convert 24 to 16 bit colours
        for (uint16_t col = 0; col < w; col++) {
          b = *bptr++;
          g = *bptr++;
          r = *bptr++;
          if (bytesPerPixel == 4) {
            a = *bptr++;
            if (a == 0) {
              *tptr++ = 0x0000;
            } else {
              uint16_t res = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
              if (res == 0x0000) {
                res = 0x0001;
              }
              *tptr++ = res;
            }
          } else {
            *tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
          }
        }
        // Read any line padding
        if (padding) {
          bmpFS.read((uint8_t*)tptr, padding);
        }
        // Push the pixel row to screen, pushImage will crop the line if needed
        sprite->pushImage(x, y + h - 1 - row, w, 1, (uint16_t*)lineBuffer, 0x0000);
      }
      if (debugLog) {
        Serial.print("Loaded in "); Serial.print(millis() - startTime);
        Serial.println(" ms");
      }
    } else {
      Serial.print("BMP format not recognized: ");
      Serial.print(colorPanes);
      Serial.print(" ");
      Serial.print(bitDepth);
      Serial.print(" ");
      Serial.println(compression);
    }
  } else {
    Serial.print("Wrong file format: ");
    Serial.println(headerBytes);
  }
  bmpFS.close();
}


void drawProgressBarCommon(DISPLAY_T* display, uint16_t percent, uint16_t greenOffset, int16_t x, int16_t y, int16_t w, int16_t h) {
  display->drawRect(x, y, w, h, TFT_WHITE);

  uint16_t fillWidth = (w * percent) / 100;

  uint16_t color;
  uint16_t percentOffset = abs(100-percent);
  if (percentOffset < greenOffset) {
    color = TFT_GREEN;
  } else if (percentOffset < 25) {
    color = TFT_YELLOW;
  } else if (percentOffset < 45) {
    color = TFT_ORANGE;
  } else {
    color = TFT_RED;
  }
  display->fillRect(x+1, y+1, fillWidth-2, h-2, color);
}

void drawProgressBar(DISPLAY_T* display, uint16_t percent, uint16_t greenOffset, int16_t x, int16_t y, int16_t w, int16_t h) {
  drawProgressBarCommon(display, percent, greenOffset, x, y, w, h);
  display->setTextSize(1);
  display->setTextColor(TFT_WHITE);
  display->setCursor(x, y - 11);
  printShaded(display, String(percent) + "%");
}

void drawProgressBar(DISPLAY_T* display, uint16_t val, uint16_t maxVal, uint16_t greenOffset, int16_t x, int16_t y, int16_t w, int16_t h) {
  uint16_t percent = (val * 100) / _max(1, maxVal);
  greenOffset = (greenOffset * 100) /_max(1, maxVal);
  drawProgressBarCommon(display, percent, greenOffset, x, y, w, h);
  display->setTextSize(1);
  display->setTextColor(TFT_WHITE);
  display->setCursor(x, y - 11);
  printShaded(display, String(val) + "/" + String(maxVal));
}

void printShaded(DISPLAY_T* display, String text) {
  printShaded(display, text, 1, 0xFFFF, 0x0000);
}

void printShaded(DISPLAY_T* display, String text, uint8_t shadeStrength) {
  printShaded(display, text, shadeStrength, 0xFFFF, 0x0000);
}

void printShaded(DISPLAY_T* display, String text, uint8_t shadeStrength, uint16_t textColor) {
  printShaded(display, text, shadeStrength, textColor, 0x0000);
}

void printShaded(DISPLAY_T* display, String text, uint8_t shadeStrength, uint16_t textColor, uint16_t shadeColor) {
  int16_t x = display->getCursorX();
  int16_t y = display->getCursorY();
  display->setTextColor(shadeColor);
  for (int16_t xOffset = -(int8_t)shadeStrength; xOffset<=shadeStrength; xOffset+=2) {
    for (int16_t yOffset = -(int8_t)shadeStrength; yOffset<=shadeStrength; yOffset+=2) {
      display->setCursor(x+xOffset, y+yOffset);
      display->print(text);
    }
  }
  display->setTextColor(textColor);
  display->setCursor(x, y);
  display->print(text);
}

void drawProfileSelectionPage(DISPLAY_T* display, uint16_t startNr, uint16_t nr, bool drawArrows, String* errorMessage) {
  int32_t columns = _min(4, nr);
  int32_t rows = nr>4 ? 2 : 1;
  int32_t cWidth = (290 - 10*columns) / columns;
  int32_t cHeight = rows==1 ? 220 : 105;
  for (int32_t c = 0; c<columns; c++) {
    for (int32_t r = 0; r<rows; r++) {
      int32_t profileId = c + r*columns;
      if (profileId < nr) {
        ProfileData profileData;
        readProfileData(profileId, &profileData, errorMessage);
        display->fillRect(20 + c*(cWidth + 10), 10+(r*(cHeight+10)), cWidth, cHeight, TFT_BLUE);
        int16_t imgW, imgH;
        getBmpDimensions(profileData.imagePath, &imgW, &imgH);
        drawBmp(profileData.imagePath, 20 + c*(cWidth + 10) + cWidth/2 - imgW/2, 10+(r*cHeight+10) + cHeight/2 - imgH/2, false);
        uint8_t textDatumBackup = display->getTextDatum();
        display->setTextDatum(BC_DATUM);
        display->setTextSize(1);
        display->drawString(profileData.name, 20 + c*(cWidth + 10) + cWidth/2, 10+(r*cHeight+10) + cHeight - 12);
        display->setTextDatum(textDatumBackup);
      }
    }
  }
}

void drawGameSelectionPage(DISPLAY_T* display, uint16_t startNr, uint16_t nr, bool drawArrows, String* errorMessage) {
  int32_t columns = _min(4, nr);
  int32_t rows = nr>4 ? 2 : 1;
  int32_t cWidth = (290 - 10*columns) / columns;
  int32_t cHeight = rows==1 ? 220 : 105; 
  for (int32_t c = 0; c<columns; c++) {
    for (int32_t r = 0; r<rows; r++) {
      if (c + r*columns < nr) {
        String gamePath = getGamePath(c + r*columns, errorMessage);
        display->fillRect(20 + c*(cWidth + 10), 10+(r*(cHeight+10)), cWidth, cHeight, TFT_BLUE);
        int16_t imgW, imgH;
        getBmpDimensions(gamePath + "logo.bmp", &imgW, &imgH);
        drawBmp(gamePath + "logo.bmp", 20 + c*(cWidth + 10) + cWidth/2 - imgW/2, 10+(r*cHeight+10) + cHeight/2 - imgH/2, false);
      }
    }
  }
}

int16_t checkSelectionPageSelection(uint16_t startNr, uint16_t nr, bool drawArrows) {
  int32_t columns = _min(4, nr);
  int32_t rows = nr>4 ? 2 : 1;
  int32_t cWidth = (290 - 10*columns) / columns;
  int32_t cHeight = rows==1 ? 220 : 105; 
  for (uint32_t c = 0; c<columns; c++) {
    for (uint32_t r = 0; r<rows; r++) {
      if (isTouchInZone(20 + c*(cWidth + 10), 10+(r*(cHeight+10)), cWidth, cHeight)) {
        return startNr + c + r*columns;
      }
    }
  }
  return -1;
}

uint16_t displayGameSelection(DISPLAY_T* display, uint16_t nr, String* errorMessage) {
  uint16_t startNr = 0;
  uint32_t ms = millis();
  uint32_t lastMs = millis();

  for (uint32_t i = 0;i<2;i++) {
    display->fillSprite(TFT_BLACK);
    drawGameSelectionPage(display, startNr, _min(nr, 8), nr>8, errorMessage);
    display->pushSpriteFast(0, 0);
  }

  while (true) {
    buttonPwr.tick();
    buttonUsr.tick();
    lastMs = ms;
    ms = millis();
    handleSerial();
    int16_t selection = checkSelectionPageSelection(startNr, _min(nr, 8), nr>8);
    if (selection != -1 && selection<nr) {
      return selection;
    }
    display->fillRect(0,0,70,20,TFT_BLACK);
    drawSystemStats(ms, lastMs);
    display->pushSpriteFast(0,0);
    if (millis()>GAME_SELECTION_POWEROFF_TIMEOUT) {
      power_off();
    }
  }
}

uint16_t displayProfileSelection(DISPLAY_T* display, uint16_t nr, String* errorMessage) {
  uint16_t startNr = 0;
  uint32_t ms = millis();
  uint32_t lastMs = millis();

  for (uint32_t i = 0;i<2;i++) {
    display->fillSprite(TFT_BLACK);
    drawProfileSelectionPage(display, startNr, _min(nr, 8), nr>8, errorMessage);
    display->pushSpriteFast(0, 0);
  }

  while (true) {
    buttonPwr.tick();
    buttonUsr.tick();
    lastMs = ms;
    ms = millis();
    handleSerial();
    int16_t selection = checkSelectionPageSelection(startNr, _min(nr, 8), nr>8);
    if (selection != -1 && selection<nr) {
      return selection;
    }
    display->fillRect(0,0,70,20,TFT_BLACK);
    drawSystemStats(ms, lastMs);
    display->pushSpriteFast(0,0);
    if (millis()>GAME_SELECTION_POWEROFF_TIMEOUT) {
      power_off();
    }
  }
}

String leftPad(String s, uint16_t len, String c) {
  while (s.length()<len) {
    s = c + s;
  }
  return s;
}

// Draws battery icon, battery voltage, FPS
void drawSystemStats(uint32_t ms, uint32_t lastMs) {
  static int32_t lowBatteryCount = -1;
  uint32_t batteryVoltage = readBatteryVoltage();
  if (lowBatteryCount == -1) { //check to run only once
    lowBatteryCount = 0;
    loadBmp(&batteryIcon[0], "/gfx/battery_low.bmp");
    loadBmp(&batteryIcon[1], "/gfx/battery_half.bmp");
    loadBmp(&batteryIcon[2], "/gfx/battery_full.bmp");
  }
  if (batteryVoltage < 3600) {
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
  if (batteryVoltage<BATTERY_LOW_WARNING_VOLTAGE && (ms/1000)%2) {
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
  spr.print(String(1000L/_max(1,ms-lastMs))); //FPS counter
  spr.setCursor(34,11);
  spr.print(String(batteryVoltage/1000) + "." + leftPad(String(batteryVoltage%1000), 3, "0") + "V"); // Battery voltage
}

void drawImageButton(DISPLAY_T* display, String path, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, uint16_t textColor) {
  display->fillRect(x, y, w, h, color);
  int16_t bmpW, bmpH;
  getBmpDimensions(path, &bmpW, &bmpH);
  drawBmp(display, path, x + w/2 - bmpW/2, y + h/2 - bmpH/2);
}

void drawButton(DISPLAY_T* display, String text, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, uint16_t textColor) {
  display->fillRect(x, y, w, h, color);
  uint8_t textDatumBak = display->getTextDatum();
  display->setTextDatum(CC_DATUM);
  uint32_t textColorBak = display->textcolor;
  display->setTextColor(textColor);
  display->drawString(text, x + w/2, y + h/2);
  display->setTextDatum(textDatumBak);
  display->setTextColor(textColorBak);
}


bool checkButton(int16_t x, int16_t y, int16_t w, int16_t h) {
  return isTouchInZone(x, y, w, h);
}


bool drawAndCheckImageButton(DISPLAY_T* display, String path, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  drawImageButton(display, path, x, y, w, h, color);
  return isTouchInZone(x, y, w, h);
}


bool drawAndCheckButton(DISPLAY_T* display, String text, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  drawButton(display, text, x, y, w, h, color);
  return isTouchInZone(x, y, w, h);
}



#define KEYBOARD_PADDING 2
#define KEYBOARD_TYPING_AREA_HEIGHT 50
#define KEYBOARD_LAYERS 1
#define KEYBOARD_ROWS 4
#define KEYBOARD_COLS 13
#define KEY_BACKSPACE 1
#define KEY_ENTER 2
#define KEY_SHIFT 3
char keyboardMatrix[KEYBOARD_LAYERS][KEYBOARD_ROWS][KEYBOARD_COLS] = {
  { // Normal layer
    {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '[', ']', KEY_BACKSPACE},
    {'q', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p', '+', '{', '}'},
    {'a', 's', 'd', 'f', 'g', 'h', 'i', 'j', 'k', 'l', '@', '~', KEY_ENTER},
    {'y', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-', ' ', '\0', KEY_SHIFT},
  }
}; 
static uint32_t currentKeyboardLayer = 0;
void drawKeyboard(DISPLAY_T* display, uint16_t keyColor, uint16_t textColor) {
  int16_t keyW = SCREEN_WIDTH/KEYBOARD_COLS;
  int16_t keyH = (SCREEN_HEIGHT-KEYBOARD_TYPING_AREA_HEIGHT)/KEYBOARD_ROWS;
  for (int16_t row=0; row<KEYBOARD_ROWS; row++) {
    for (int16_t col=0; col<KEYBOARD_COLS; col++) {
      char c = keyboardMatrix[currentKeyboardLayer][row][col];
      if (c>32) {
        String character = " ";
        character.setCharAt(0, c);
        drawButton(display, character, keyW*col+KEYBOARD_PADDING, KEYBOARD_TYPING_AREA_HEIGHT+keyH*row+KEYBOARD_PADDING, keyW-2*KEYBOARD_PADDING, keyH-2*KEYBOARD_PADDING, keyColor, textColor);
      } else if (c==KEY_BACKSPACE) {
        drawButton(display, "<-", keyW*col+KEYBOARD_PADDING, KEYBOARD_TYPING_AREA_HEIGHT+keyH*row+KEYBOARD_PADDING, keyW-2*KEYBOARD_PADDING, keyH-2*KEYBOARD_PADDING, keyColor, textColor);
      }
    }
  }
}

void checkKeyboard(DISPLAY_T* display, String* output, uint32_t maxCharacters, uint16_t keyColor, uint16_t textColor) {
  *output = "";
  int16_t keyW = SCREEN_WIDTH/KEYBOARD_COLS;
  int16_t keyH = (SCREEN_HEIGHT-KEYBOARD_TYPING_AREA_HEIGHT)/KEYBOARD_ROWS;
  while (output->length()<maxCharacters) {
    for (int16_t row=0; row<KEYBOARD_ROWS; row++) {
      for (int16_t col=0; col<KEYBOARD_COLS; col++) {
        if (checkButton(keyW*col, KEYBOARD_TYPING_AREA_HEIGHT+keyH*row, keyW, keyH)) {
          display->fillSprite(TFT_BLACK);
          drawKeyboard(display, keyColor, textColor);
          *output = *output + keyboardMatrix[currentKeyboardLayer][row][col];
          uint8_t textDatumBak = display->getTextDatum();
          display->setTextDatum(TL_DATUM);
          uint32_t textColorBak = display->textcolor;
          display->setTextColor(textColor);
          display->drawString(*output, 10, 10);
          Serial.print("Drawing output: ");
          Serial.println(*output);
          display->setTextDatum(textDatumBak);
          display->setTextColor(textColorBak);
          display->pushSpriteFast(0,0);
          while (touch.pressed()) {}
        }
      }
    }
  }
}

void checkFailWithMessage(String message) {
  if (!message.isEmpty()) {
    tft.fillScreen(TFT_BLACK);
    spr.fillSprite(TFT_BLACK);
    spr.setCursor(1, 16);
    spr.println("FEHLER:");
    spr.println(message);
    spr.pushSprite(0, 0);
    while (true) {
      handleSerial();
    };
  }
}