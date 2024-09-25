#ifndef __WIFI_HANDLER_H__
#define __WIFI_HANDLER_H__

#include "Arduino.h"
#include "prefsHandler.h"
#include <WiFi.h>
#include "constants.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"

#define CONNECTION_OK 0
#define CONNECTION_NOWIFI 1
#define CONNECTION_NOTRAMPOLINE 2
#define CONNECTION_NOTCONNECTED 3

#define WIFI_RETRY_COUNT 10
#define TRAMPOLINE_RETRY_COUNT 3

uint8_t connectToTrampoline();
void getJumpData(JumpData* jumpData);




#endif /* __WIFI_HANDLER_H__ */