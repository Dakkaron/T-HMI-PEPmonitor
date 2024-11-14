#include "serialHandler.h"

void handleUploadFileMode() {
  char serialBuffer[512];
  String path;
  uint32_t fileLength = 0;
  uint32_t serialCharacterCount = 0;
  while (true) { //  Read file path
    if (Serial.available() > 0) {
      char readCharacter = Serial.read();
      serialBuffer[serialCharacterCount++] = readCharacter;
      if (readCharacter == 0) {
        path = serialBuffer;
        break;
      }
    }
  }
  while (true) { //  Read file length
    if (Serial.available() > 0) {
      char readCharacter = Serial.read();
      serialBuffer[serialCharacterCount++] = readCharacter;
      if (readCharacter == 0) {
        fileLength = atoi(serialBuffer);
        break;
      }
    }
  }
  uint32_t bytesRead = 0;
  File file = SD_MMC.open(path, "w", true);
  while (bytesRead<fileLength) {
    if (Serial.available() > 0) {
      file.write(Serial.read());
      bytesRead++;
    }
  }
  file.close();
  Serial.println("Done writing file");
}

void handlePrintVersion() {
  Serial.print("PEPit Version '");
  Serial.print(VERSION);
  Serial.println("'");
}

char serialCommandBuffer[16];
uint32_t serialCommandCharacterCount = 0;
void handleSerial() {
  if (Serial.available() > 0) {
    char charRead = Serial.read();
    serialCommandBuffer[serialCommandCharacterCount++] = charRead;
    //Serial.print(charRead);
    serialCommandBuffer[serialCommandCharacterCount] = 0;
    if (strcmp(serialCommandBuffer, "UL ") == 0) {
      handleUploadFileMode();
    } else if (strcmp(serialCommandBuffer, "VER ") == 0) {
      handlePrintVersion();
    }

    if (charRead == 32 || charRead == 0) { // Check for space or 0
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
