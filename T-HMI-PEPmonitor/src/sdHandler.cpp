#include "sdHandler.h"

String readFileToString(const char *path) {
  File file = SD_MMC.open(path);
  if (!file) {
    Serial.print("Failed to open file");
    Serial.print(path);
    Serial.println(" for reading");
    return "";
  }

  String s;
  while (file.available()) {
    char charRead = file.read();
    s += charRead;
  }
  return s;
}

String readFileLineToString(const char *path, uint32_t lineNr) {
  File file = SD_MMC.open(path);
  if (!file) {
    Serial.print("Failed to open file");
    Serial.print(path);
    Serial.println(" for reading");
    return "";
  }

  uint32_t currentLineNr = 0;
  String s;
  while (file.available()) {
    char charRead = file.read();
    if (charRead == '\n') {
      currentLineNr++;
    } else if (currentLineNr == lineNr) {
      s += charRead;
    } else if (currentLineNr > lineNr) {
      break;
    }
  }
  return s;
}

void writeStringToFile(const char *path, String val) {
  File file = SD_MMC.open(path);
  if (!file) {
    Serial.print("Failed to open file");
    Serial.print(path);
    Serial.println(" for writing!");
  }
  for (uint32_t i; i<val.length(); i++) {
    file.write(val[i]);
  }
}

void writeIntToFile(const char *path, int32_t val) {
  writeStringToFile(path, String(val));
}

int32_t readIntFromFile(const char *path) {
  return readIntFromFile(path, 0);
}

int32_t readIntFromFile(const char *path, uint32_t lineNr) {
  uint16_t line = 0;
  String data = readFileLineToString(path, lineNr);
  if (data.equals("")) {
    return 0;
  }
  return data.toInt();
}