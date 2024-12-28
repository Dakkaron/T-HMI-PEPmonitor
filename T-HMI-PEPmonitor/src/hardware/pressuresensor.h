#ifndef __PRESSURESENSOR_H__
#define __PRESSURESENSOR_H__

#include <Arduino.h>
#include "config.h"
#include <Adafruit_HX711.h>
#include "constants.h"

void readPressure(Adafruit_HX711* hx711, BlowData* blowData);
extern Adafruit_HX711 hx711;

#endif /*PRESSURESENSOR*/