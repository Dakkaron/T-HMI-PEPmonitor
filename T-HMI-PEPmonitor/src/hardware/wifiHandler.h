#ifndef __WIFI_HANDLER_H__
#define __WIFI_HANDLER_H__

#include "Arduino.h"
#include <WiFi.h>
#include "constants.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "sdHandler.h"

#define CONNECTION_OK 0
#define CONNECTION_NOWIFI 1
#define CONNECTION_NOTRAMPOLINE 2

#define WIFI_RETRY_COUNT 10
#define TRAMPOLINE_RETRY_COUNT 3

uint8_t connectToTrampoline();
void getJumpData(JumpData* jumpData);

bool startFetchingNTPTime();
void getNTPTime(String* ntpDateString, String* ntpTimeString, String* errorMessage);



#endif /* __WIFI_HANDLER_H__ */