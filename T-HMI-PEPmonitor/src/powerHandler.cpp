#include "powerHandler.h"

uint32_t readBatteryVoltage() {
    return (analogRead(BAT_ADC_PIN) * 162505) / 100000;
}

void power_off() {
    digitalWrite(PWR_ON_PIN, LOW);
    Serial.println("power_off");
}