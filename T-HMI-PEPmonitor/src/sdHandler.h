#ifndef __SDHANDLER_H__
#define __SDHANDLER_H__
#include <Arduino.h>
#include <SD_MMC.h>
#include <FS.h>
#include <SPI.h>
#include <SD.h>
#include "constants.h"

#define SYSTEM_CONFIG_INI_PATH "/systemConfig.ini"
#define INI_BUFFER_LEN 128

#define WIN_SCREEN_PATH "/gfx/win"

void getIniSection(String iniPath, String section, char* resultBuffer, uint16_t len);
String getIniValueFromSection(char* sectionData, String key);
bool getIniValue(String iniPath, String section, String key, char* resultBuffer, uint16_t len);
void readSystemConfig();
String getRandomWinScreenPath();
String readFileToString(const char *path);
String readFileLineToString(const char *path, uint32_t lineNr);
void writeStringToFile(const char *path, String val);
void writeIntToFile(const char *path, int32_t val);
int32_t readIntFromFile(const char *path);
int32_t readIntFromFile(const char *path, uint32_t lineNr);

#endif /* __SDHANDLER_H__*/