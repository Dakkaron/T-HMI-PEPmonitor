#ifndef __SDHANDLER_H__
#define __SDHANDLER_H__
#include <Arduino.h>
#include <SD_MMC.h>
#include <FS.h>
#include <SPI.h>
#include <SD.h>
#include "constants.h"

#define SYSTEM_CONFIG_INI_PATH "/systemConfig.ini"
#define PROFILE_DATA_INI_PATH "/profiles.ini"
#define INI_BUFFER_LEN 128

#define WIN_SCREEN_PATH "gfx/win"
#define GAMES_ROOT_DIR "/games"

uint16_t getNumberOfGames(String* errorMessage);
String getGamePath(uint16_t gameId, String* errorMessage);
uint32_t getNumberOfProfiles(String* errorMessage);
void readProfileData(uint32_t profileId, ProfileData* profileData, String* errorMessage);

void readGameConfig(String gamePath, GameConfig* gameConfig, String* errorMessage);

void getIniSection(String iniPath, String section, char* resultBuffer, uint16_t len, String* errorMessage);
bool isKeyInSection(char* sectionData, String key);
String getIniValueFromSection(char* sectionData, String key, String* errorMessage);

String getRandomWinScreenPath(String gamePath, String* errorMessage);

String readFileToString(const char *path);
String readFileLineToString(const char *path, uint32_t lineNr);
void writeStringToFile(const char *path, String val);
void writeIntToFile(const char *path, int32_t val);
int32_t readIntFromFile(const char *path);
int32_t readIntFromFile(const char *path, uint32_t lineNr);

#endif /* __SDHANDLER_H__*/