#ifndef FEET_H
#define FEET_H

#include <Accelerometer.h>

class Feet
{
private:
    Accelerometer *accel;

public:
    Feet(Accelerometer *_accel)
    {
        accel = _accel;
    }
    void update()
    {
    }

    void run()
    {
    }

    bool isStepDetected()
    {   
    }

    double getStepPower()
    {
    }

    bool getStepDirection() //are we need this?
    {
    }
};

#endif