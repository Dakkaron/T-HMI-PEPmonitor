#ifndef __GFXHANDLER_H__
#define __GFXHANDLER_H__
#include <Arduino.h>
#include <TFT_eSPI.h>

#include <SD_MMC.h>
#include <FS.h>
#include <SPI.h>
#include <SD.h>
#include <algorithm>
#include "touchHandler.h"
#include "sdHandler.h"

#define DISPLAY_T TFT_eSprite

extern TFT_eSPI tft;
extern TFT_eSprite spr;

#define FLIPPED_H 0x01
#define FLIPPED_V 0x02

void getBmpDimensions(String filename, int16_t* w, int16_t* h);
void drawBmp(String filename, int16_t x, int16_t y, bool debugLog = true);
void loadBmp(DISPLAY_T* display, String filename);
void loadBmp(DISPLAY_T* display, String filename, uint8_t flipped);
void loadBmpAnim(DISPLAY_T** display, String filename, uint8_t animFrames);
void loadBmpAnim(DISPLAY_T** display, String filename, uint8_t animFrames, uint8_t flipped);
void drawProgressBar(DISPLAY_T* display, uint16_t progress, uint16_t greenOffset, int16_t x, int16_t y, int16_t w, int16_t h);
void drawProgressBar(DISPLAY_T* display, uint16_t val, uint16_t maxVal, uint16_t greenOffset, int16_t x, int16_t y, int16_t w, int16_t h);
void printShaded(DISPLAY_T* display, String text, uint8_t shadeStrength, uint16_t textColor, uint16_t shadeColor);
void printShaded(DISPLAY_T* display, String text, uint8_t shadeStrength, uint16_t textColor);
void printShaded(DISPLAY_T* display, String text, uint8_t shadeStrength);
void printShaded(DISPLAY_T* display, String text);
uint16_t displayGameSelection(DISPLAY_T* display, uint16_t nr, String* errorMessage);

#endif /*__GFXHANDLER_H__*/