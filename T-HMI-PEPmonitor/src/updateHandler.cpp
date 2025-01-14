#include "updateHandler.h"
#include "Update.h"
#include "hardware/wifiHandler.h"
#include "hardware/sdHandler.h"
#include "hardware/gfxHandler.hpp"
#include "SD_MMC.h"

static String systemUpdatePath = "";
static bool systemUpdateWasChecked = false;
static bool systemUpdateCheckTriggered = false;
TaskHandle_t firmwareCheckTaskHandle;

void firmwareCheckTask(void* parameter) {
  Serial.println("Checking for firmware update in task");
  getSystemUpdatePath();
  vTaskDelete(NULL);
}

uint8_t getSystemUpdateAvailableStatus() {
  if (!systemUpdateCheckTriggered) {
    Serial.println("Starting system update check task");
    systemUpdateCheckTriggered = true;
    xTaskCreatePinnedToCore(firmwareCheckTask, "updateCheck", 10000, NULL, 1, &firmwareCheckTaskHandle, 0);
  }
  if (!systemUpdateWasChecked) {
    return FIRMWARE_UPDATE_CHECK_RUNNING;
  } else if (systemUpdatePath.length() == 0) {
    return FIRMWARE_UPDATE_NOT_AVAILABLE;
  } else {
    return FIRMWARE_UPDATE_AVAILABLE;
  }
}

String getSystemUpdatePath() {
  while (systemUpdateCheckTriggered && !systemUpdateWasChecked && !(xTaskGetCurrentTaskHandle() == firmwareCheckTaskHandle)) {
    Serial.println("loop");
    delay(10);
  }
  if (systemUpdateWasChecked) {
    return systemUpdatePath;
  }
  Serial.println("Checking for firmware update");
  String ignoreMessage;
  String url = downloadFileToString(FIRMWARE_RELEASE_PATH_URL, &ignoreMessage);
  if (ignoreMessage.length() > 0) {
    Serial.println("Error checking for firmware update: "+ignoreMessage);
    systemUpdateWasChecked = true;
    return "";
  }
  String lastUrl = "";
  if (SD_MMC.exists("/lastFirmwareUrl.txt")) {
    lastUrl = readFileToString("/lastFirmwareUrl.txt");
  }
  systemUpdatePath = url.equals(lastUrl) ? "" : url;
  systemUpdateWasChecked = true;
  Serial.println("Result: "+systemUpdatePath);
  return systemUpdatePath;
}

void downloadAndRunSystemUpdate(String url, String* errorMessage) {
  if (url.equals("")) {
    Serial.println("No new firmware update available.");
    displayFullscreenMessage("Kein neues Firmware-Update\nverfügbar.");
    delay(5000);
    return;
  }
  displayFullscreenMessage("Firmware-Download\nwird initialisiert...");
  Serial.println("Downloading firmware update from "+url);
  downloadFile(url, "/firmware.bin", errorMessage, [](int progress, int total) {
    displayFullscreenMessage("Update herunterladen\n\n    "+String(progress / (total / 100))+"%");
    Serial.printf("Progress: %d%%\r", (progress / (total / 100)));
  });
  if (errorMessage->length() > 0) {
    return;
  }
  Update.onProgress([](int progress, int total) {
    displayFullscreenMessage("Update installieren\n\n    "+String(progress / (total / 100))+"%");
    Serial.printf("Progress: %d%%\r", (progress / (total / 100)));
  });
  displayFullscreenMessage("Update wird\nangewendet...");
  Serial.println("Starting update");
  File fileStream = SD_MMC.open("/firmware.bin");
  Update.begin(fileStream.size(), U_FLASH);
  Update.writeStream(fileStream);
  if (Update.end()) {
    displayFullscreenMessage("Update erfolgreich!\n\nWird neugestartet...");
    Serial.println("Update done");
    writeStringToFile("/lastFirmwareUrl.txt", url);
    delay(2000);
    Serial.println("Resetting device");
    ESP.restart();
  } else {
    errorMessage->concat("Update failed: ");
    errorMessage->concat(Update.errorString());
    errorMessage->concat("\n");
    return;
  }
}