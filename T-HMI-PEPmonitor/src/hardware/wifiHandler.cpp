#include "wifiHandler.h"

HTTPClient http;
String ssid = "";
String password = "";
String trampolineIp = "";

uint8_t startWifi() {
  static bool wifiConnected = false;
  if (wifiConnected) {
    return CONNECTION_OK;
  }
  Serial.println("Starting WIFI");
  WiFi.mode(WIFI_STA);
  ssid = systemConfig.wifiSsid;
  password = systemConfig.wifiPassword;
  trampolineIp = systemConfig.trampolineIp;
  Serial.println(ssid);
  Serial.println(password);
  Serial.println(trampolineIp);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WIFI");
  for (uint8_t i=0;i<WIFI_RETRY_COUNT;i++) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.print('.');
      delay(1000);
    } else {
      wifiConnected = true;
      break;
    }
  }
  if (wifiConnected) {
    Serial.println("done");
    return CONNECTION_OK;
  } else {
    Serial.println("Failed to connect to WIFI");
    return CONNECTION_NOWIFI;
  }
}

uint8_t connectToTrampoline() {
  if (systemConfig.simulateTrampoline) {
    return CONNECTION_OK;
  } else {
    startWifi();
    Serial.println("done");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("Trampoline URL: ");
    Serial.println(("http://" + trampolineIp + "/resetCurrentSession"));

    Serial.print("Connecting to trampoline ");
    bool connectionFound = false;
    for (uint8_t i=0; i<CONNECTION_NOTRAMPOLINE; i++) {
      http.begin(("http://" + trampolineIp + "/resetCurrentSession").c_str());
      int httpResponseCode = http.GET();
      String result = http.getString();
      if (httpResponseCode == 200 && result == "Done") {
        Serial.println("done");
        connectionFound = true;
        break;
      } else {
        Serial.print(".");
      }
      if (!connectionFound) {
        Serial.println("Failed to connect to trampoline");
        Serial.println(httpResponseCode);
        Serial.println(result);
      }
    }
    if (connectionFound) {
      Serial.println("Connected to trampoline successfully");
    }
    return connectionFound ? CONNECTION_OK : CONNECTION_NOTRAMPOLINE;
  }
}

JsonDocument jsonDocument;
unsigned long simulateStartMs = 0;
void getJumpData(JumpData* jumpData) {
  if (systemConfig.simulateTrampoline) {
    if (simulateStartMs==0) {
      simulateStartMs = millis();
    }
    jumpData->ms = millis();
    jumpData->jumpCount = (jumpData->ms - simulateStartMs) / 1000;
    jumpData->currentlyJumping = true;
    jumpData->msLeft = simulateStartMs + jumpData->totalTime - jumpData->ms;
    jumpData->misses = 0;
    jumpData->highscore = 250;
    jumpData->newHighscore = jumpData->jumpCount > 250;
    jumpData->lastReadSuccessful = true;
  } else {
    http.begin(("http://" + trampolineIp + "/jumpReport").c_str());
    int httpResponseCode = http.GET();
    String result = http.getString();

    if (httpResponseCode != 200) {
      Serial.print("getJumpReport(): HTTP ERROR ");
      Serial.println(httpResponseCode);
      Serial.println(result);
      jumpData->lastReadSuccessful = false;
      return;
    }
    deserializeJson(jsonDocument, result);
    jumpData->ms = millis();
    jumpData->jumpCount = jsonDocument["jumpCount"];
    jumpData->currentlyJumping = jsonDocument["currentlyJumping"];
    jumpData->msLeft = jsonDocument["msLeft"];
    jumpData->misses = jsonDocument["misses"];
    jumpData->highscore = jsonDocument["highscore"];
    jumpData->newHighscore = jsonDocument["newHighscore"];
    jumpData->lastReadSuccessful = true;
  }

  if (systemConfig.debugLogTrampoline) {
    Serial.println("New jump data:");
    Serial.print("jumpCount: ");
    Serial.println(jumpData->jumpCount);
    Serial.print("currentlyJumping: ");
    Serial.println(jumpData->currentlyJumping);
    Serial.print("msLeft: ");
    Serial.println(jumpData->msLeft);
    Serial.print("misses: ");
    Serial.println(jumpData->misses);
    Serial.print("highscore: ");
    Serial.println(jumpData->highscore);
    Serial.print("newHighscore: ");
    Serial.println(jumpData->newHighscore);
  }
}

bool startFetchingNTPTime() {
  uint32_t connectionStatus = startWifi();
  if (connectionStatus == CONNECTION_OK) {
    Serial.println("Starting NTP");
    configTime(systemConfig.timezoneOffset, 0, "pool.ntp.org", "time.nist.gov");
    return true;
  }
  Serial.println("Failed to start NTP because of missing WIFI connection");
  return false;
}

String getNTPTime(String* errorMessage) {
  /*if (time(nullptr)) {
    Serial.println("Time is null pointer");
    return "";
  }*/
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time from NTP");
    errorMessage->concat("Konnte Zeit nicht per NTP abrufen.\n");
    errorMessage->concat("Wahrscheinlich WLAN-Verbindungsproblem.\n");
    return "";
  }
  return String(timeinfo.tm_mday) + "." + String(timeinfo.tm_mon) + "." + String(timeinfo.tm_year+1900) + " " + String(timeinfo.tm_hour) + ":" + String(timeinfo.tm_min);
}