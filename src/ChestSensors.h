#ifndef CHEST_ACCEL_H
#define CHEST_ACCEL_H

#include <Arduino.h>
#include <SoftwareSerial.h>
class ChestSensors
{
private:
    SoftwareSerial *mySerial;

    int roll;
    int pitch;
    double accelZ;
    double altitude;

public:
    ChestSensors()
    {
        mySerial = new SoftwareSerial(3, 2);
       // mySerial->begin(9600);
    }

    int getRoll()
    {
        if (roll < -180)
            return roll + 360;
        else
            return roll;
    }

    int getPitch()
    {
        if (pitch < -180)
            return pitch + 360;
        else
            return pitch;
    }

    double getAccelZ()
    {
        return accelZ;
    }
    double getAltitude()
    {
        return altitude;
    }

    bool update()
    {
        if (mySerial->available())
        {
            String data = mySerial->readStringUntil('\n');

            if (data.length() < 4)
                return false;

            pitch = data.substring(0, data.indexOf(",")).toInt();
            data.remove(0, data.indexOf(",") + 1);
            roll = data.substring(0, data.indexOf(",")).toInt();
            data.remove(0, data.indexOf(",") + 1);
            accelZ = data.substring(0, data.indexOf(",")).toDouble();
            data.remove(0, data.indexOf(",") + 1);
            altitude = data.substring(0, data.indexOf(",")).toDouble();
            return true;
        }
        else
            return true;
    }
};

#endif