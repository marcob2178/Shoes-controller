#ifndef WEIGHT_SENSOR_H
#define WEIGHT_SENSOR_H
#include <Arduino.h>

class WeightSensor
{
private:
    uint8_t analogPin;
    /* data */
public:
    WeightSensor(uint8_t pin)
    {
        analogPin = pin;
    }

    int readRaw(){
        return analogRead(analogPin);
    }
};

#endif