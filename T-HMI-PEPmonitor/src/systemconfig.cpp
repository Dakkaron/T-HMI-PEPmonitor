#include "systemconfig.h"
#include "hardware/sdHandler.h"

SystemConfig systemConfig = SystemConfig();

void initSystemConfig(String* errorMessage) {
  readSystemConfig(&systemConfig, errorMessage);
}