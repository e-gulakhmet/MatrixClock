#ifndef _MAIN_H_
#define _MAIN_H_

#include <Arduino.h>

const uint8_t BUTT_1_PIN = 4;
const uint8_t BUTT_2_PIN = 7;
const uint8_t BUTT_3_PIN = 8;
const uint8_t MATRIX_CS_PIN = 10;
const uint8_t POWER_SENSOR_PIN = A2;
const uint8_t LIGHT_SENSOR_PIN = A3;

typedef enum {
    mmClock,
    mmDate,
    mmTemp,
    mmPower
} MainMode;


#endif // _MAIN_H_