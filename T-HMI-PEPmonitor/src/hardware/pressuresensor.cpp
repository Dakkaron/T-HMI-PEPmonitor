#include "hardware/pressuresensor.h"

Adafruit_HX711 hx711 = Adafruit_HX711(HX7711_DATA_PIN, HX7711_CLOCK_PIN);

uint32_t readings[PRESSURE_SENSOR_SMOOTHING_NUM_READINGS];
uint8_t readIndex = 0;
uint32_t total = 0;
uint8_t skips = 0;
void readPressure(Adafruit_HX711* hx711, BlowData* blowData) {
    if (blowData->ms + 4999 < millis()) {
      for (int i=0;i<PRESSURE_SENSOR_SMOOTHING_NUM_READINGS;i++) {
        readings[i] = 0;
      }
    }
  if (systemConfig.simulateBlows) {
    uint32_t blowDuration = blowData->ms - blowData->blowStartMs;
    uint8_t isBlowing = (blowDuration) < (blowData->targetDurationMs+100) ||
                        (blowDuration) > (blowData->targetDurationMs+100+SIMULATE_BLOWS_PAUSE_DURATION);
    blowData->pressure = isBlowing ? blowData->targetPressure : 1; 
  } else {
    if (hx711->isBusy()) {
      return;
    }
    int32_t sensorValue = (hx711->readChannel(CHAN_A_GAIN_64) / (PRESSURE_SENSOR_DIVISOR * blowData->targetPressure));
    if (systemConfig.logBlowPressure) {
      Serial.print(F("Channel A (Gain 64): "));
      Serial.print(sensorValue);
      Serial.print(F(" / "));
    }
    if (sensorValue >= -PRESSURE_SENSOR_CUTOFF_LIMIT && sensorValue <= PRESSURE_SENSOR_CUTOFF_LIMIT) {
      if (blowData->negativePressure) {
        sensorValue = -sensorValue;
      }
      sensorValue = _max(0, sensorValue);
      skips = 0;
      total = total - readings[readIndex];
      readings[readIndex] = sensorValue;
      total = total + readings[readIndex];
      readIndex = readIndex + 1;
      if (readIndex >= PRESSURE_SENSOR_SMOOTHING_NUM_READINGS) {
        readIndex = 0;
      }
      blowData->pressure = total / PRESSURE_SENSOR_SMOOTHING_NUM_READINGS;
      if ((blowData->ms - blowData->blowStartMs) < blowData->targetDurationMs) {
        blowData->cumulativeError = abs(((int32_t)blowData->pressure - 100)) + ((blowData->cumulativeError * (PRESSURE_SENSOR_CUMULATIVE_ERROR_FACTOR-1)) / PRESSURE_SENSOR_CUMULATIVE_ERROR_FACTOR);
      }
    } else {
      skips++;
      if (skips > 10) {
        skips--;
        total = total - readings[readIndex];
        readings[readIndex] = 0;
        total = total + readings[readIndex];
        readIndex = readIndex + 1;
        if (readIndex >= PRESSURE_SENSOR_SMOOTHING_NUM_READINGS) {
          readIndex = 0;
        }
        blowData->pressure = total / PRESSURE_SENSOR_SMOOTHING_NUM_READINGS;
        if ((blowData->ms - blowData->blowStartMs) < blowData->targetDurationMs) {
          blowData->cumulativeError = abs(((int32_t)blowData->pressure - 100)) + ((blowData->cumulativeError * (PRESSURE_SENSOR_CUMULATIVE_ERROR_FACTOR-1)) / PRESSURE_SENSOR_CUMULATIVE_ERROR_FACTOR);
        }
      }
    }
  }
  if (systemConfig.logBlowPressure) {
    Serial.println(blowData->pressure);
  }
}