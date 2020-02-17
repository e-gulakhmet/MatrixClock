#ifndef _POWER_H_
#define _POWER_H_

#include <Arduino.h>



class Power{
    public:
        Power(uint8_t pin);
        void update();
        uint8_t getVoltage() {return voltage_;}
        uint8_t getProcent() {return procent_;}
        void setMinVolt(uint8_t min_volt);
        void setMaxVolt(uint8_t max_volt);
    
    private:
        float readVcc(); 
        float readAnalog(); // Функция считывания опорного напряжения

        uint8_t pin_;
        uint8_t voltage_;
        uint8_t procent_;
        uint8_t min_volt_;
        uint8_t max_volt_;
};


#endif // _POWER_H_