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
    double distance = 0;

    double velocity = 0;
    double buffer[3][20];
    int bufferIndex = 0;

public:
    Foot(Accelerometer *_accel, WeightSensor *_sideSensor, WeightSensor *_backSensor)
    {
        accel = _accel;
        sideSensor = _sideSensor;
        backSensor = _backSensor;
        // buffer[0][0] = 0;

        for (int i = 0; i < 20; i++)
        {
            buffer[0][i] = 0;
            buffer[1][i] = 0;
            buffer[2][i] = 0;
        }
    }

    // right

    double getWalkingPower()
    {
        return (rMovementCount / (rTimeCounter / CALCULATING_PERIOD));
    }

    double getRawPower()
    {
        return rMovementCount;
    }

    double getVelocity()
    {
        return velocity;
    }

    double getDistance()
    {
        return distance;
    }

    long getStepTime()
    {
        return rTimeCounter;
    }

    bool isCruiseControl()
    {
        return rTimeCounter > 1000;
    }

    int getCruiseControlPower()
    {
        return abs((int)accel->getRoll());
    }

    void updateBuffer() //stream lifo buffer
    {
        buffer[0][bufferIndex] = accel->getLinAccel().x();
        buffer[1][bufferIndex] = accel->getLinAccel().y();
        buffer[2][bufferIndex] = accel->getLinAccel().z();

        bufferIndex++;
        if (bufferIndex == 20)
        {
            bufferIndex = 0;
        }

        // for (int i = 0; i < 20; i++)
        // {
        //     Serial.print(buffer[2][i] + String("\t"));
        // }
        // Serial.println();
    }

    void processPreData()
    {
        for (int i = 0; i < 20; i++)
        {
            bufferIndex++;
            if (bufferIndex == 20)
            {
                bufferIndex = 0;
            }
            
            rTimeCounter += CALCULATING_PERIOD;
            rMovementCount += buffer[2][bufferIndex] > 0 ? buffer[2][bufferIndex] : -buffer[2][bufferIndex];

            velocity += 0.01 * buffer[2][bufferIndex];
            distance += 0.01 * velocity;

        }
    }

    bool isWalking()
    {
        updateBuffer();

        rCurStepState = -accel->getRoll();
        //  calculate duration of step

        if (rCurStepState >= FEET_ANGLE)
        {
            if (rTimeCounter == 0)
            { //means at the start of the step
                processPreData();
            }
            else
            {
                rTimeCounter += CALCULATING_PERIOD;
                double val = accel->getLinAccel().z();
                rMovementCount += val > 0 ? val : -val;

                velocity += 0.01 * -val;
                distance += 0.01 * velocity;
            }
        }

        if ((rCurStepState < FEET_ANGLE) && (rPrevStepState >= FEET_ANGLE))
        {
            rMovementCount = 0;
            rTimeCounter = 0;
            distance = 0;
            velocity = 0;
        }
        rPrevStepState = rCurStepState;
        return rTimeCounter > 0;
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

    static double mapDouble(double x, double in_min, double in_max, double out_min, double out_max)
    {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
};

#endif