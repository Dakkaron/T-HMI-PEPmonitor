#include "powerHandler.h"
#include "gfxHandler.hpp"
#include "serialHandler.h"

OneButton buttonPwr = OneButton(BUTTON2_PIN, false, false);
OneButton buttonUsr = OneButton(BUTTON1_PIN, false, false);

uint32_t readBatteryVoltage() {
    return (analogRead(BAT_ADC_PIN) * 162505) / 100000;
}

void power_off() {
    digitalWrite(PWR_ON_PIN, LOW);
    Serial.println("power_off");
    
    uint32_t lastMs = millis();
    uint32_t ms = millis();
    while (true) {
        digitalWrite(PWR_ON_PIN, LOW);
        spr.fillSprite(TFT_BLACK);
        drawSystemStats(ms, lastMs);
        lastMs = ms;
        ms = millis();
        spr.pushSpriteFast(0,0);
        handleSerial();
    }
}