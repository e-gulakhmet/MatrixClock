#ifndef _MAIN_H_
#define _MAIN_H_

#include <Arduino.h>

const uint8_t BUTT_1_PIN = A0;
const uint8_t BUTT_2_PIN = A1;
const uint8_t MATRIX_CS_PIN = 9;

typedef enum {
    mmClock,
    mmTemp
} MainMode;


#endif // _MAIN_H_