#ifndef __SDHANDLER_H__
#define __SDHANDLER_H__
#include <Arduino.h>
#include <SD_MMC.h>
#include <FS.h>
#include <SPI.h>
#include <SD.h>

String readFileToString(const char *path);
String readFileLineToString(const char *path, uint32_t lineNr);
void writeStringToFile(const char *path, String val);
void writeIntToFile(const char *path, int32_t val);
int32_t readIntFromFile(const char *path);
int32_t readIntFromFile(const char *path, uint32_t lineNr);

#endif /* __SDHANDLER_H__*/