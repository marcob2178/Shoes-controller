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

    int getX()
    {
        return roll;
    }

    int getY()
    {
        return pitch;
    }

    double getAccelZ(){
        return accelZ;
    }

    void update()
    {
        if (mySerial->available())
        {
            String data = mySerial->readStringUntil('\n');
            //Serial.println(data);
            pitch = data.substring(0, data.indexOf(",")).toInt();
            data.remove(0, data.indexOf(",") + 1);
            roll = data.substring(0, data.indexOf(",")).toInt();
            data.remove(0, data.indexOf(",") + 1);
            accelZ = data.substring(0, data.indexOf(",")).toDouble();
        }
    }
};

#endif