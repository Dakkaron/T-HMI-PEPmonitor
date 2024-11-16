#include "serialHandler.h"


void readFileToSerial(const char *path, bool readAsHex, bool printFileSize) {
  Serial.printf("Reading file: %s\n", path);

  File file = SD_MMC.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  uint8_t byteNr = 0;
  if (printFileSize) {
    Serial.println(file.size());
  }
  Serial.println("Read from file: ");
  while (file.available()) {
    if (readAsHex) {
      Serial.print(file.read(), 16);
      Serial.print(" ");
      byteNr++;
      if (byteNr % 16 == 0) {
        Serial.println();
      }
    } else {
      Serial.write(file.read());
    }
  }
}

bool dirExists(const char *dirname) {
  File root = SD_MMC.open(dirname);
  if (!root) {
    return false;
  }
  return root.isDirectory();
}


void listDir(const char *dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);

  File root = SD_MMC.open(dirname);
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
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(file.path(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

String readSerialLine() {
  String serialRead;
  while (true) { //  Read file path
    if (Serial.available() > 0) {
      char readCharacter = Serial.read();
      Serial.print(readCharacter);
      if (readCharacter == 0 || readCharacter == 10 || readCharacter == 13) {
        return serialRead;
      }
      serialRead += readCharacter;
    }
  }
}

void handleUploadFileMode() {
  Serial.println("Upload mode");
  String path = readSerialLine();
  Serial.print("Writing to file: '");
  Serial.print(path);
  Serial.println("'");

  uint32_t fileLength = atoi(readSerialLine().c_str());
  Serial.print("Number of bytes: '");
  Serial.print(fileLength);
  Serial.println("'");

  uint32_t bytesRead = 0;
  if (SD_MMC.exists(path)) {
    Serial.println("Deleting existing file");
    SD_MMC.remove(path);
  }

  File file = SD_MMC.open(path, FILE_WRITE, true);
  while (bytesRead<fileLength) {
    if (Serial.available() > 0) {
      char readCharacter = Serial.read();
      file.print(readCharacter);
      Serial.print(readCharacter);
      bytesRead++;
    }
  }
  file.close();
  Serial.println();
  Serial.println("Done writing file");
}

void handlePrintVersion() {
  Serial.print("PEPit Version '");
  Serial.print(VERSION);
  Serial.println("'");
}

void handleLs() {
  String line = readSerialLine();
  if (line.indexOf(" ")==-1) {
    listDir(line.c_str(), 0);
  } else {
    String path = line.substring(0,line.indexOf(" "));
    uint32_t levels = atoi(line.substring(line.indexOf(" ")+1).c_str());
    listDir(path.c_str(), levels);
  }
}

char serialCommandBuffer[16];
uint32_t serialCommandCharacterCount = 0;
void handleSerial() {
  if (Serial.available() > 0) {
    char charRead = Serial.read();
    serialCommandBuffer[serialCommandCharacterCount++] = charRead;
    Serial.print(charRead);
    serialCommandBuffer[serialCommandCharacterCount] = 0;
    if (strcasecmp(serialCommandBuffer, "ul ") == 0) {
      handleUploadFileMode();
    } else if (strcasecmp(serialCommandBuffer, "ver ") == 0) {
      handlePrintVersion();
    } else if (strcasecmp(serialCommandBuffer, "ls ") == 0) {
      handleLs();
    } else if (strcasecmp(serialCommandBuffer, "ls\n") == 0) {
      listDir("/", 0);
    } else if (strcasecmp(serialCommandBuffer, "cat ") == 0) {
      readFileToSerial(readSerialLine().c_str(), false, false);
    } else if (strcasecmp(serialCommandBuffer, "catl ") == 0) {
      readFileToSerial(readSerialLine().c_str(), false, true);
    } else if (strcasecmp(serialCommandBuffer, "catx ") == 0) {
      readFileToSerial(readSerialLine().c_str(), true, false);
    } else if (strcasecmp(serialCommandBuffer, "catxl ") == 0) {
      readFileToSerial(readSerialLine().c_str(), true, true);
    }

    if (charRead == 32 || charRead == 0 || charRead == 10) { // Check for space, \n or \0
      for (uint32_t i=0;i<16;i++) {
        serialCommandBuffer[i] = 0;
      }
      serialCommandCharacterCount = 0;
    }

    if (serialCommandCharacterCount>15) {
      serialCommandCharacterCount = 0;
    }
  }
}
