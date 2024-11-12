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
  Serial.println("Done writing file");
}

char serialCommandBuffer[16];
uint32_t serialCommandCharacterCount = 0;
void checkForDownloadMode() {
  if (Serial.available() > 0) {
    serialCommandBuffer[serialCommandCharacterCount++] = Serial.read();
    serialCommandBuffer[serialCommandCharacterCount] = 0;
    if (strcmp(serialCommandBuffer, "UL") == 0) {
      handleUploadFileMode();
    }
    if (serialCommandCharacterCount>15) {
      serialCommandCharacterCount = 0;
    }
  }
}
