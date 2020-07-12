#ifndef CHEST_ACCEL_H
#define CHEST_ACCEL_H

#include <Arduino.h>
#include <SoftwareSerial.h>
class ChestAccel
{
private:
    SoftwareSerial *mySerial;

    int roll;
    int pitch;
    double accelZ;

public:
    ChestAccel()
    {
        mySerial = new SoftwareSerial(3, 2);
        mySerial->begin(9600);
    }

    int getRoll()
    {
        return roll;
    }

    int getPitch()
    {
        return pitch;
    }

    double getAccelZ()
    {
        return accelZ;
    }

    bool update()
    {
        if (mySerial->available())
        {
            String data = mySerial->readStringUntil('\n');

            if (data.length() < 3)
                return false;
            //Serial.println(data);
            pitch = data.substring(0, data.indexOf(",")).toInt();
            data.remove(0, data.indexOf(",") + 1);
            roll = data.substring(0, data.indexOf(",")).toInt();
            data.remove(0, data.indexOf(",") + 1);
            accelZ = data.substring(0, data.indexOf(",")).toDouble();
            return true;
        }
         else 
         return true;
    }
};

#endif