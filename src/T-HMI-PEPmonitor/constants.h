#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#define LAST_BLOW_SUCCEEDED 0B00000001
#define LAST_BLOW_FAILED    0B00000010
#define NEW_BLOW            0B00000100

#define PRESSURE_BAR_WIDTH 180
#define PRESSURE_BAR_HEIGHT 10
#define PRESSURE_BAR_X 1
#define PRESSURE_BAR_Y 190

#define PRESSURE_SENSOR_LONG_BLOW_DIVISOR  11000L //(32768L/3L)
#define PRESSURE_SENSOR_SHORT_BLOW_DIVISOR 20000L //(32768L/6L)
#define PRESSURE_SENSOR_CUTOFF_LIMIT 500
#define PRESSURE_SENSOR_BLOWING_THRESHOLD 45
#define PRESSURE_SENSOR_SMOOTHING_NUM_READINGS 10
#define PRESSURE_SENSOR_MAX_SKIPS 10;

#define PRESSURE_SENSOR_CUMULATIVE_ERROR_FACTOR 16

#define SCREEN_BRIGHTNESS_STEPS 16
#define SCREEN_ROTATION 3
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define HX7711_DATA_PIN 17
#define HX7711_CLOCK_PIN 18

#define SIMULATE_BLOWS_SHORT_BLOW_DURATION 1000
#define SIMULATE_BLOWS_PAUSE_DURATION 5000

#define WIN_SCREEN_COUNT 23

struct BlowData {
  bool currentlyBlowing = false;
  unsigned long ms = 0;
  unsigned long blowStartMs = 0;
  unsigned long blowEndMs = 0;
  uint8_t cycleNumber = 0;
  uint8_t blowCount = 0;
  uint32_t pressure = 0;
  uint16_t maxPressure = 0;
  uint16_t cumulativeError = 0;
  uint8_t fails = 0;
  bool isLongBlows = true;
  uint8_t lastBlowStatus = 0;
};

#endif /* __CONSTANTS_H__ */