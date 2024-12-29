#ifndef __SYSTEMCONFIG_H__
#define __SYSTEMCONFIG_H__

#include <Arduino.h>

struct SystemConfig {
  String trampolineIp;
  String wifiSsid;
  String wifiPassword;
  uint8_t touchScreenZThreshold;
  bool simulateTrampoline;
  bool simulateBlows;
  bool simulateInhalation;
  bool logBlowPressure;
  bool logTrampoline;
};

extern SystemConfig systemConfig;

void initSystemConfig(String* errorMessage);

#endif /* __SYSTEMCONFIG_H__ */