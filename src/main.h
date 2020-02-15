#ifndef _MAIN_H_
#define _MAIN_H_

#include <Arduino.h>

const uint8_t BUTT_1_PIN = A0;
const uint8_t BUTT_2_PIN = A1;
const uint8_t MATRIX_CS_PIN = 10;
const uint8_t POWER_SENSOR_PIN = A2;

typedef enum {
    mmClock,
    mmDate,
    mmTemp,
    mmPower
} MainMode;


#endif // _MAIN_H_