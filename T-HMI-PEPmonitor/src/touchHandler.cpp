#include "touchHandler.h"

#include "pins.h"
#include <Arduino.h>

touch_calibration_t calibration_data[4];

XPT2046 touch = XPT2046(SPI, TOUCHSCREEN_CS_PIN, TOUCHSCREEN_IRQ_PIN);

void initTouch() {
  //initTouchCalibration();
  //readTouchCalibration(calibration_data);
  SPI.begin(TOUCHSCREEN_SCLK_PIN, TOUCHSCREEN_MISO_PIN, TOUCHSCREEN_MOSI_PIN);
  touch.begin(240, 320);
  touch.setCal(1788, 285, 1877, 311, 240, 320);
  //touch.setCal(calibration_data[0].rawX, calibration_data[2].rawX, calibration_data[0].rawY, calibration_data[2].rawY, 240, 320);
  touch.setRotation((SCREEN_ROTATION + 2) % 4);
  touch.setZThreshold(TOUCH_SCREEN_Z_THRESHOLD);
}

bool isTouchInZone(int16_t x, int16_t y, int16_t w, int16_t h) {
  int16_t tx = touch.X();
  int16_t ty = touch.Y();
  int16_t tz = touch.RawZ();
  Serial.print("Touch: ");
  Serial.print(tx);
  Serial.print("/");
  Serial.print(ty);
  Serial.print("/");
  Serial.println(tz);
  return (tx>=x && tx<=x+w && ty>=y && ty<=y+h);
}
