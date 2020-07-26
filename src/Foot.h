#ifndef FEET_H
#define FEET_H

#include <Accelerometer.h>

class Foot
{
private:
    Accelerometer *accel;

public:
    Foot(Accelerometer *_accel)
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
};

#endif