#include "pressuresensor.h"

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
  #ifdef SIMULATE_BLOWING
    uint32_t blowDuration = blowData->ms - blowData->blowStartMs;
    uint8_t isBlowing = (blowDuration) < (blowData->isLongBlows ? LONG_BLOW_DURATION_MS+100 : SIMULATE_BLOWS_SHORT_BLOW_DURATION+100) ||
                        (blowDuration) > (blowData->isLongBlows ? LONG_BLOW_DURATION_MS+SIMULATE_BLOWS_PAUSE_DURATION : SIMULATE_BLOWS_SHORT_BLOW_DURATION+SIMULATE_BLOWS_PAUSE_DURATION);
    blowData->pressure = isBlowing ? SHORT_BLOW_MIN_STRENGTH + 5 : 1; 
  #else // !SIMULATE_BLOWING
    int32_t sensorValue = (hx711->readChannelBlocking(CHAN_A_GAIN_64) / (blowData->isLongBlows ? PRESSURE_SENSOR_LONG_BLOW_DIVISOR : PRESSURE_SENSOR_SHORT_BLOW_DIVISOR)); // Dropping the least significant 15 bits
    #ifdef LOG_BLOW_PRESSURE
      Serial.print(F("Channel A (Gain 64): "));
      Serial.print(sensorValue);
      Serial.print(F(" / "));
    #endif
    if (sensorValue >= -PRESSURE_SENSOR_CUTOFF_LIMIT && sensorValue <= PRESSURE_SENSOR_CUTOFF_LIMIT) {
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
      if ((blowData->ms - blowData->blowStartMs) < LONG_BLOW_DURATION_MS) {
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
        if ((blowData->ms - blowData->blowStartMs) < LONG_BLOW_DURATION_MS) {
          blowData->cumulativeError = abs(((int32_t)blowData->pressure - 100)) + ((blowData->cumulativeError * (PRESSURE_SENSOR_CUMULATIVE_ERROR_FACTOR-1)) / PRESSURE_SENSOR_CUMULATIVE_ERROR_FACTOR);
        }
      }
    }
  #endif // SIMULATE_BLOWING
  #ifdef LOG_BLOW_PRESSURE
    Serial.println(blowData->pressure);
  #endif
}