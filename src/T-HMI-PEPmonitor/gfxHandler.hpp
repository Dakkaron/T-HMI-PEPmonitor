#ifndef __GFXHANDLER_H__
#define __GFXHANDLER_H__
#include <Arduino.h>
#include <TFT_eSPI.h>

#include <SD_MMC.h>
#include <FS.h>
#include <SPI.h>
#include <SD.h>
#include <algorithm>

#define DISPLAY_T TFT_eSprite

extern TFT_eSPI tft;
extern TFT_eSprite spr;

#define FLIPPED_H 0x01
#define FLIPPED_V 0x02

void drawBmp(String filename, int16_t x, int16_t y);
void loadBmp(DISPLAY_T* display, String filename);
void loadBmp(DISPLAY_T* display, String filename, uint8_t flipped);
void loadBmpAnim(DISPLAY_T** display, String filename, uint8_t animFrames);
void loadBmpAnim(DISPLAY_T** display, String filename, uint8_t animFrames, uint8_t flipped);
void drawProgressBar(DISPLAY_T* display, uint16_t progress, int16_t x, int16_t y, int16_t w, int16_t h);
void drawProgressBar(DISPLAY_T* display, uint16_t val, uint16_t maxVal, int16_t x, int16_t y, int16_t w, int16_t h);

#endif /*__GFXHANDLER_H__*/