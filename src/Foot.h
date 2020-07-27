#ifndef FEET_H
#define FEET_H

#include <Accelerometer.h>
#include <WeightSensor.h>
#include <settings.h>

class Foot
{
private:
    Accelerometer *accel;
    WeightSensor *sideSensor;
    WeightSensor *backSensor;

    bool isRStepDone = false;
    int rCurStepState = 0, rPrevStepState = 0, rStepsCount = 0;

    long rTimeCounter = 0;
    double rMovementCount = 0;

public:
    Foot(Accelerometer *_accel, WeightSensor *_sideSensor, WeightSensor *_backSensor)
    {
        accel = _accel;
        sideSensor = _sideSensor;
        backSensor = _backSensor;
    }

    // right

    double getWalkingPower()
    {
        return (rMovementCount / (rTimeCounter / 33.0));
    }

    bool isCruiseControl()
    {
        return rTimeCounter > 1000;
    }

    int getCruiseControlPower()
    {
        return abs((int)accel->getRoll());
    }

    bool isWalking()
    {
        isRStepDone = false;
        rCurStepState = -accel->getRoll();
        //  calculate duration of step

        if (rCurStepState >= FEET_ANGLE)
        {
            rTimeCounter += 33;
            rMovementCount += accel->getLinAccel().z() > 0 ? accel->getLinAccel().z() : -accel->getLinAccel().z();
        }

        if ((rCurStepState < FEET_ANGLE) && (rPrevStepState >= FEET_ANGLE))
        {
            isRStepDone = true;

            rMovementCount = 0;
            rTimeCounter = 0;
        }
        rPrevStepState = rCurStepState;

        return rTimeCounter > 0;

        //  then we need process the acceleration data througth the "moving average algorithm" for 16 values
        //  after that we measure the acceleration power and calculate step power
    }

    int getStepBackPower()
    {
        return backSensor->readRaw();
    }

    bool isStepBack()
    {
        return getStepBackPower() > 200;
    }

    //
    //  sidestep

    int getSidePower()
    {
        return sideSensor->readRaw();
    }

    bool isSideStep()
    {
        return getSidePower() > 200;
    }

    // tools

    double mapDouble(double x, double in_min, double in_max, double out_min, double out_max)
    {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
};

#endif