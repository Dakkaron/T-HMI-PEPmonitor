#ifndef Adafruit_HX711_h
#define Adafruit_HX711_h

#include "Arduino.h"

enum hx711_chanGain_t {
  CHAN_A_GAIN_128 = 25,
  CHAN_A_GAIN_64 = 27,
  CHAN_B_GAIN_32 = 26
};

/**
 * @brief Library for the HX711 load cell amplifier.
 */
class Adafruit_HX711 {

public:
  Adafruit_HX711(uint8_t dataPin, uint8_t clockPin);
  void begin();
  bool isBusy();
  void powerDown(bool down);
  int32_t readChannel(hx711_chanGain_t chanGain = CHAN_A_GAIN_128);
  int32_t readChannelRaw(hx711_chanGain_t chanGain = CHAN_A_GAIN_128);
  int32_t readChannelBlocking(hx711_chanGain_t chanGain = CHAN_A_GAIN_128);
  void tareA(int32_t tareValue);
  void tareB(int32_t tareValue);

private:
  uint8_t _dataPin;  ///< Data pin
  uint8_t _clockPin; ///< Clock pin
  int32_t readRawData(uint8_t pulses);
  int32_t _tareValueA; ///< Tare offset value for Channel A
  int32_t _tareValueB; ///< Tare offset value for Channel B
};

#endif
