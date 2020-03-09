#include <power.h>



Power::Power(uint8_t power_pin, uint8_t battery_pin)
    : power_pin_(power_pin)
    , battery_pin_(battery_pin)
    , is_charging(false)
    , voltage_(0)
    , procent_(0)
    , min_volt_(3.3)
    , max_volt_(4.2)
    {
        pinMode(power_pin_, INPUT);
        pinMode(battery_pin_, INPUT);
    };



void Power::update() {
    if (analogRead(power_pin_) > 600) {
        is_charging = true;
    }
    else{
        is_charging = false;
    }    
    // Синхронизируем полученные данные о напряжении с фильтром
    voltage_ = (readAnalog() * readFilterVcc()) / 1023;
    // Получаем процент заряда, зависящий от минимального напряжения и максимального
    procent_ = (voltage_ - min_volt_) / (max_volt_ - min_volt_) * 100;
}



void Power::setMinVolt(float min_volt) {
    if (min_volt < 0) {
        min_volt_ = 0;
    }
    else {
        min_volt_ = min_volt;
    }
}



void Power::setMaxVolt(float max_volt) {
    max_volt_ = max_volt;
}



float Power::readFilterVcc() {
    // read multiple values and sort them to take the mode
    float sortedValues[100];
    for (int i = 0; i < 100; i++) {
        float tmp = 0.0;
        ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
        ADCSRA |= _BV(ADSC); // Start conversion
        delay(25);
        while (bit_is_set(ADCSRA, ADSC)); // measuring
        uint8_t low = ADCL; // must read ADCL first - it then locks ADCH
        uint8_t high = ADCH; // unlocks both
        tmp = (high << 8) | low;
        float value = (1.11 * 1023.0) / tmp;
        int j;
        if (value < sortedValues[0] || i == 0) {
            j = 0; //insert at first position
        }
        else {
            for (j = 1; j < i; j++) {
                if (sortedValues[j - 1] <= value && sortedValues[j] >= value) {
                    // j is insert position
                    break;
                }
            }
        }
        for (int k = i; k > j; k--) {
            // move all values higher than current reading up one position
            sortedValues[k] = sortedValues[k - 1];
        }
        sortedValues[j] = value; //insert current reading
    }
    //return scaled mode of 10 values
    float returnval = 0;
    for (int i = 100 / 2 - 5; i < (100 / 2 + 5); i++) {
        returnval += sortedValues[i];
    }
    return returnval / 10;
}



float Power::readAnalog() {
    // read multiple values and sort them to take the mode
    int sortedValues[100];
    for (int i = 0; i < 100; i++) { 
        delay(25);
        int value = analogRead(battery_pin_);
        int j;
        if (value < sortedValues[0] || i == 0) {
            j = 0; //insert at first position
        }
        else {
            for (j = 1; j < i; j++) {
                if (sortedValues[j - 1] <= value && sortedValues[j] >= value) {
                    // j is insert position
                    break;
                }
            }
        }
        for (int k = i; k > j; k--) {
            // move all values higher than current reading up one position
            sortedValues[k] = sortedValues[k - 1];
        }
        sortedValues[j] = value; //insert current reading
    }
    //return scaled mode of 10 values
    float returnval = 0;
    for (int i = 100 / 2 - 5; i < (100 / 2 + 5); i++) {
        returnval += sortedValues[i];
    }
    return returnval / 10;
}



