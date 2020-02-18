#ifndef _POWER_H_
#define _POWER_H_

#include <Arduino.h>



class Power{
    public:
        Power(uint8_t pin);
        void update();
        float getVoltage();
        uint8_t getProcent();
        void setMinVolt(uint8_t min_volt);
        void setMaxVolt(uint8_t max_volt);
    
    private:
        float readFilterVcc(); 
        float readAnalog(); // Функция считывания опорного напряжения


        uint8_t pin_;
        uint8_t min_volt_;
        uint8_t max_volt_;
};


#endif // _POWER_H_