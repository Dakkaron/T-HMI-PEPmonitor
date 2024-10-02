#include "sdHandler.h"
#include "gfxHandler.hpp"

uint32_t numberOfWinScreens;

void getIniSection(String iniPath, String section, char* resultBuffer, uint16_t len) {
  char lineBuffer[INI_BUFFER_LEN];
  section = section + "\n";

  File file = SD_MMC.open(iniPath);
  if (!file) {
    Serial.print("Failed to open INI file ");
    Serial.println(iniPath);
    return;
  }
  uint16_t i = 0;
  char* resultLinePointer = resultBuffer;
  bool inCorrectSection = false;
  while (file.available()) {
    lineBuffer[i] = file.read();
    lineBuffer[i+1] = 0;
    if (lineBuffer[i] == '\n' || lineBuffer[i] == '\r') {
      lineBuffer[i] = '\n';
      if (inCorrectSection) {
        if (lineBuffer[0] == '[') {
          return; // Correct section finished
        }
        strcpy(resultLinePointer, lineBuffer);
        resultLinePointer += i+1;
        resultLinePointer[0] = 0;
      } else if (lineBuffer[0] == '[') {
        if (strcmp(section.c_str(), lineBuffer) == 0) {
          inCorrectSection = true;
        }
      }
      i = -1;
      lineBuffer[0] = 0;
    }
    i++;
  }
}

String getIniValueFromSection(char* sectionData, String key) {
  int16_t lineStartMarker;
  int16_t keyEndMarker = -1;
  int16_t valStartMarker = -1;
  int16_t i = 0;
  char buffer[INI_BUFFER_LEN];
  while (sectionData[i]!=0) {
    if (sectionData[i] == '\n') {
      if (keyEndMarker!=-1) {
        if (strncmp(key.c_str(), sectionData + lineStartMarker, keyEndMarker - lineStartMarker) == 0) {
          strncpy(buffer, sectionData + valStartMarker, i-valStartMarker);
          buffer[i-valStartMarker] = 0;
          return String(buffer);
        }
      }
      lineStartMarker = i+1;
      keyEndMarker = -1;
      valStartMarker = -1;
    } else if (sectionData[i] == '=') {
      uint16_t v = i-1;
      while (v>0 && (sectionData[v] == ' ' || sectionData[v] == '\t')) {
        v--;
      }
      keyEndMarker = v;
      v = i+1;
      while (sectionData[v] == ' ' || sectionData[v] == '\t') {
        v++;
      }
      valStartMarker = v;
    }
    i++;
  }
  return String("");
}

void scanForWinScreens() {
  numberOfWinScreens = 0;
  Serial.printf("Listing directory: %s\n", WIN_SCREEN_PATH);

  File root = SD_MMC.open(WIN_SCREEN_PATH);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (!file.isDirectory()) {
      numberOfWinScreens++;
    }
    file = root.openNextFile();
  }
}

String getRandomWinScreenPath() {
  if (numberOfWinScreens == 0) {
    scanForWinScreens();
  }
  uint32_t selectedWinScreenNumber = random(0,numberOfWinScreens);
  Serial.printf("Listing directory: %s\n", WIN_SCREEN_PATH);

  File root = SD_MMC.open(WIN_SCREEN_PATH);
  if (!root) {
    Serial.println("Failed to open directory");
    return "";
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return "";
  }

  uint32_t i = 0;
  File file = root.openNextFile();
  while (file) {
    if (!file.isDirectory()) {
      if (i == selectedWinScreenNumber) {
        return String(WIN_SCREEN_PATH) + "/" + file.name();
      }
      i++;
    }
    file = root.openNextFile();
  }
  return "";
}


void readSystemConfig() {
}

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