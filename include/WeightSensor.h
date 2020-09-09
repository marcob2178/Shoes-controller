#ifndef WEIGHT_SENSOR_H
#define WEIGHT_SENSOR_H
#include <Arduino.h>

class WeightSensor
{
private:
    uint8_t analogPin;
    int data = 0;
    /* data */
public:
    WeightSensor(uint8_t pin)
    {
        analogPin = pin;
    }
    
    void update(){
        data = analogRead(analogPin);
    }

    int readRaw(){
        return data;
    }
};

#endif