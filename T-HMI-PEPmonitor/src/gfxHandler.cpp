#include "gfxHandler.hpp"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);

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


void drawBmp(String filename, int16_t x, int16_t y) {
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
        tft.pushImage(0, h - 1 - row, w, 1, (uint16_t*)lineBuffer);
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