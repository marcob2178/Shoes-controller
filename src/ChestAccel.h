#ifndef CHEST_ACCEL_H
#define CHEST_ACCEL_H

#include <Arduino.h>
#include <SoftwareSerial.h>
class ChestAccel
{
private:
    SoftwareSerial *mySerial;

    int y3_axis_1;
    int y3_axis_2;
    int y3_thres_1;
    int y3_thres_2;
    int pox;

public:
    ChestAccel()
    {
        mySerial = new SoftwareSerial(3, 2);
        mySerial->begin(9600);
    }

    int getX()
    {
        return y3_axis_1;
    }

    int getY()
    {
        return y3_axis_2;
    }

    void update()
    {
        if (mySerial->available())
        {
            String data = mySerial->readStringUntil('\n');
            //Serial.println(data);
            y3_axis_1 = data.substring(0, data.indexOf(",")).toInt();
            data.remove(0, data.indexOf(",") + 1);
            y3_axis_2 = data.substring(0, data.indexOf(",")).toInt();
            data.remove(0, data.indexOf(",") + 1);
            y3_thres_1 = data.substring(0, data.indexOf(",")).toInt();
            data.remove(0, data.indexOf(",") + 1);
            y3_thres_2 = data.substring(0, data.indexOf(",")).toInt();
            data.remove(0, data.indexOf(",") + 1);
            pox = data.substring(0, data.length()).toInt();
        }
    }
};

#endif