#ifndef _MAIN_H_
#define _MAIN_H_

#include <Arduino.h>

const uint8_t LEFT_BUTT_PIN = 4;
const uint8_t MAIN_BUTT_PIN = 2;
const uint8_t RIGHT_BUTT_PIN = 8;
const uint8_t MATRIX_CS_PIN = 10;
const uint8_t POWER_SENSOR_PIN = A3;
const uint8_t LIGHT_SENSOR_PIN = A0;
const uint8_t PIR_SENSOR_PIN = 3;

typedef enum {
    mmClock,
    mmDate,
    mmTemp,
    mmPower
} MainMode;

typedef enum {
    pmPower,
    pmNormal,
    pmEco
} PowerMode;


#endif // _MAIN_H_