#include "sdHandler.h"
#include "gfxHandler.hpp"

uint32_t numberOfWinScreens;

uint16_t getNumberOfGames(String* errorMessage) {
  Serial.println("getNumberOfGames()");
  File root = SD_MMC.open(GAMES_ROOT_DIR);

  if (!root) {
    errorMessage->concat("Failed to open /games directory!");
    Serial.println(*errorMessage);
    return 0;
  }
  if (!root.isDirectory()) {
    errorMessage->concat("/games is not a directory!");
    Serial.println(*errorMessage);
    return 0;
  }
  File file = root.openNextFile();
  uint16_t gameCount = 0;
  while (file) {
    if (file.isDirectory()) {
      Serial.print("Found game directory ");
      Serial.print(String(GAMES_ROOT_DIR) + "/" + file.name() + "/" + "gameconfig.ini");
      File gameIni = SD_MMC.open(String(GAMES_ROOT_DIR) + "/" + file.name() + "/" + "gameconfig.ini");
      Serial.print(", ");
      if (gameIni && !gameIni.isDirectory()) {
        gameCount++;
        Serial.println("CONFIRMED!");
      } else {
        Serial.println("NOT A GAME!");
      }
    }
    file = root.openNextFile();
  }
  return gameCount;
}

String getGamePath(uint16_t gameId, String* errorMessage) {
  Serial.println("getGamePath()");
  File root = SD_MMC.open(GAMES_ROOT_DIR);

  if (!root) {
    errorMessage->concat("Failed to open /games directory!");
    Serial.println(*errorMessage);
    return "";
  }
  if (!root.isDirectory()) {
    errorMessage->concat("/games is not a directory!");
    Serial.println(*errorMessage);
    return "";
  }
  File file = root.openNextFile();
  uint16_t gameCount = 0;
  while (file) {
    if (file.isDirectory()) {
      File gameIni = SD_MMC.open(String(GAMES_ROOT_DIR) + "/" + file.name() + "/" + "gameconfig.ini");
      if (gameIni && !gameIni.isDirectory()) {
        if (gameCount == gameId) {
          Serial.print("Game ini returned: ");
          Serial.println(String(GAMES_ROOT_DIR) + "/" + file.name() + "/");
          return String(GAMES_ROOT_DIR) + "/" + file.name() + "/";
        }
        gameCount++;
      }
    }
    file = root.openNextFile();
  }
  return "";
}

void readGameConfig(String gamePath, GameConfig* gameConfig, String* errorMessage) {
  char resBuffer[1024];
  getIniSection(gamePath+"gameconfig.ini", "[game]", resBuffer, 1024, errorMessage);
  gameConfig->name = getIniValueFromSection(resBuffer, "name", errorMessage);
  gameConfig->templateName = getIniValueFromSection(resBuffer, "template", errorMessage);
  gameConfig->prefsNamespace = getIniValueFromSection(resBuffer, "prefsNamespace", errorMessage);
}

void getIniSection(String iniPath, String section, char* resultBuffer, uint16_t len, String* errorMessage) {
  char lineBuffer[INI_BUFFER_LEN];
  if (section.c_str()[0] != '[') {
    section = "[" + section + "]";
  }
  section = section + "\n";

  File file = SD_MMC.open(iniPath);
  if (!file) {
    errorMessage->concat("Failed to open INI file ");
    errorMessage->concat(iniPath);
    Serial.println(*errorMessage);
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
  if (!inCorrectSection) {
    errorMessage->concat("Section ");
    errorMessage->concat(section);
    errorMessage->concat(" not found in INI file ");
    errorMessage->concat(iniPath);
    Serial.println(*errorMessage);
  }
}

String getIniValueFromSection(char* sectionData, String key, String* errorMessage) {
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
  errorMessage->concat("Key ");
  errorMessage->concat(key);
  errorMessage->concat(" not found in INI section.");
  Serial.println(*errorMessage);
  return String("");
}

void scanForWinScreens(String gamePath, String* errorMessage) {
  String gameWinScreenDir = gamePath + WIN_SCREEN_PATH;
  numberOfWinScreens = 0;
  Serial.printf("Searching directory for winscreens: %s\n", gameWinScreenDir.c_str());

  File root = SD_MMC.open(gameWinScreenDir.c_str());
  if (!root) {
    errorMessage->concat("Failed to open directory ");
    errorMessage->concat(gameWinScreenDir);
    Serial.println(*errorMessage);
    return;
  }
  if (!root.isDirectory()) {
    errorMessage->concat(gameWinScreenDir);
    errorMessage->concat(" is not a directory!");
    Serial.println(*errorMessage);
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

String getRandomWinScreenPath(String gamePath, String* errorMessage) {
  String gameWinScreenDir = gamePath + WIN_SCREEN_PATH;
  if (numberOfWinScreens == 0) {
    scanForWinScreens(gamePath, errorMessage);
  }
  uint32_t selectedWinScreenNumber = random(0,numberOfWinScreens);
  Serial.printf("Getting winscreen path from directory: %s\n", gameWinScreenDir.c_str());

  File root = SD_MMC.open(gameWinScreenDir.c_str());
  if (!root) {
    errorMessage->concat("Failed to open directory ");
    errorMessage->concat(gameWinScreenDir);
    Serial.println(*errorMessage);
    return "";
  }
  if (!root.isDirectory()) {
    errorMessage->concat(gameWinScreenDir);
    errorMessage->concat(" is not a directory!");
    Serial.println(*errorMessage);
    return "";
  }

  uint32_t i = 0;
  File file = root.openNextFile();
  while (file) {
    if (!file.isDirectory()) {
      if (i == selectedWinScreenNumber) {
        return gameWinScreenDir + "/" + file.name();
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