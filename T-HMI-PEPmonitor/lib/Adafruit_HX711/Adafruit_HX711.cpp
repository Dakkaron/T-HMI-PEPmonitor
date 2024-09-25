#include "Adafruit_HX711.h"

/**
 * @brief Construct a new Adafruit HX711 object
 *
 * @param dataPin Data pin number
 * @param clockPin Clock pin number
 */
Adafruit_HX711::Adafruit_HX711(uint8_t dataPin, uint8_t clockPin) {
  _dataPin = dataPin;
  _clockPin = clockPin;
}

/**
 * @brief Initialize the HX711 module
 * Sets up the pin modes and performs a power reset
 */
void Adafruit_HX711::begin() {
  pinMode(_dataPin, INPUT);
  pinMode(_clockPin, OUTPUT);
  powerDown(true);  // Perform a power reset
  delay(1);         // Hold pin high for 1 ms for reset
  powerDown(false); // Wake up
}

/**
 * @brief Power down or wake up the HX711
 *
 * @param down true to power down, false to wake up
 */
void Adafruit_HX711::powerDown(bool down) {
  digitalWrite(_clockPin, down ? HIGH : LOW);
}

/**
 * @brief Blocking read value from the specified channel and gain
 *
 * @param chanGain Channel and gain configuration
 * @return int32_t The reading from the sensor
 */
int32_t Adafruit_HX711::readChannelBlocking(hx711_chanGain_t chanGain) {
  readChannel(chanGain); // First, set the desired gain and discard this read
  return readChannel(chanGain); // Now perform the actual read
}

/**
 * @brief Read data from the HX711, handling channel and gain setup, and with
 * 'tare' offset
 *
 * @param chanGain Channel and gain configuration
 * @return int32_t The signed 32-bit extended raw sensor data
 */
int32_t Adafruit_HX711::readChannel(hx711_chanGain_t chanGain) {
  return readChannelRaw(chanGain) -
         (chanGain == CHAN_B_GAIN_32 ? _tareValueB : _tareValueA);
}

/**
 * @brief Read data from the HX711, handling channel and gain setup, NO tare
 * offset - the 'raw' ADC value from the HX!
 *
 * @param chanGain Channel and gain configuration
 * @return int32_t The signed 32-bit extended raw sensor data
 */
int32_t Adafruit_HX711::readChannelRaw(hx711_chanGain_t chanGain) {
  while (isBusy())
    ; // Wait until the HX711 is ready

  digitalWrite(_clockPin, LOW);
  uint32_t value = 0;
  for (int i = 0; i < 24; i++) { // Read 24 bits from DOUT
    digitalWrite(_clockPin, HIGH);
    delayMicroseconds(1);
    value = (value << 1) | digitalRead(_dataPin);
    digitalWrite(_clockPin, LOW);
    delayMicroseconds(1);
  }

  // Set gain for next reading
  for (int i = 0; i < chanGain - 24; i++) {
    digitalWrite(_clockPin, HIGH);
    digitalWrite(_clockPin, LOW);
  }

  // Convert to 32-bit signed integer
  if (value & 0x800000)
    value |= 0xFF000000;

  return (int32_t)value;
}

/**
 * @brief Check if the HX711 is busy
 *
 * @return true If the HX711 is currently busy (DOUT is high)
 * @return false If the HX711 is ready for data retrieval (DOUT is low)
 */
bool Adafruit_HX711::isBusy() { return digitalRead(_dataPin) == HIGH; }

/**
 * @brief Set the raw 'tare' offset for channel A
 * @param tareValue Signed i32 that is added to channel A readings
 */
void Adafruit_HX711::tareA(int32_t tareValue) {
  _tareValueA = tareValue; // Set the tare offset value for Channel A
}

/**
 * @brief Set the raw 'tare' offset for channel B
 * @param tareValue Signed i32 that is added to channel B readings
 */
void Adafruit_HX711::tareB(int32_t tareValue) {
  _tareValueB = tareValue; // Set the tare offset value for Channel B
}
