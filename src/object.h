#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <Arduino.h>


const byte procent[8] = {
    B100111,
    B100101,
    B010111,
    B001000,
    B000100,
    B111010,
    B101001,
    B111001
};


const byte gradus[3] = {
    B111,
    B101,
    B111,
};


const byte C[8] = {
    B0110,
    B1001,
    B1001,
    B0001,
    B0001,
    B1001,
    B1001,
    B0110
};


const byte sun[2][7] = {
    {
    B0001000,
    B0101010,
    B0011100,
    B1111111,
    B0011100,
    B0101010,
    B0001000,
    }
    ,
    {
    B1000001,
    B0101010,
    B0011100,
    B0111110,
    B0011100,
    B0101010,
    B1000001,
    }
};



const byte cloud[2][8] = {
    {
    B01111110,
    B11111111,
    B11111111,
    B01111110,
    B01000010,
    B00001000,
    B10010001,
    B01000100
    }
    ,
    {
    B01111110,
    B11111111,
    B11111111,
    B01111110,
    B10000100,
    B01010001,
    B00000100,
    B10010010
    }
};

#endif // _OBJECT_H_