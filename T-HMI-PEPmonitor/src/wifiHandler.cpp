#include "wifiHandler.h"

HTTPClient http;
String ssid = "";
String password = "";
String trampolineIp = "";

uint8_t connectToTrampoline() {
#ifdef SIMULATE_TRAPOLINE
  return CONNECTION_OK;
#else
  WiFi.mode(WIFI_STA);
  prefs.begin("wifi");
  ssid = prefs.getString("ssid", "");
  password = prefs.getString("pass", "");
  trampolineIp = prefs.getString("tIp");
  Serial.println(ssid);
  Serial.println(password);
  Serial.println(trampolineIp);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WIFI");
  bool wifiConnected = false;
  for (uint8_t i=0;i<WIFI_RETRY_COUNT;i++) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.print('.');
      delay(1000);
    } else {
      wifiConnected = true;
      break;
    }
  }
  if (!wifiConnected) {
    return CONNECTION_NOWIFI;
  }
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
#endif
}

JsonDocument jsonDocument;
unsigned long simulateStartMs = 0;
void getJumpData(JumpData* jumpData) {
  #ifdef SIMULATE_TRAPOLINE
    if (simulateStartMs==0) {
      simulateStartMs = millis();
    }
    jumpData->ms = millis();
    jumpData->jumpCount = (jumpData->ms - simulateStartMs) / 1000;
    jumpData->currentlyJumping = true;
    jumpData->msLeft = simulateStartMs + (5L * 60L * 1000L) - jumpData->ms;
    jumpData->misses = 0;
    jumpData->highscore = 250;
    jumpData->newHighscore = jumpData->jumpCount > 250;
    jumpData->lastReadSuccessful = true;
  #else
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
  #endif

  #ifdef DEBUG_LOG_TRAMPOLINE
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
  #endif
}