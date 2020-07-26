#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <Adafruit_BNO055.h>

class Accelerometer
{
private:
    Adafruit_BNO055 *bno;
    imu::Vector<3> euler;
    imu::Vector<3> linaccel;

    double offset_y, offset_z;

    //  x       y       z
    //  yaw     pitch   roll

    void setOffsets(double _offset_y, double _offset_z)
    {
        offset_y = _offset_y;
        offset_z = _offset_z;
    }

public:
    Accelerometer(int id, int address)
    {
        bno = new Adafruit_BNO055(id, address);
    };

    void calibrate()
    {
        euler = bno->getVector(Adafruit_BNO055::VECTOR_EULER);
        setOffsets(euler.y(), euler.z());
    }

    void begin()
    {
        if (!bno->begin())
        {
            Serial.print("Ooops, no BNO055 1 detected ... Check your wiring or I2C ADDR!");
            while (1)
                ;
        }
        delay(500);

        //bno->setExtCrystalUse(true);
        delay(500);
        
        bno->printSensorDetails();
        calibrate();
    }

    imu::Vector<3> getLinAccel()
    {
        return linaccel;
    }

    double getYaw()
    {
        return euler.x();
    }

    double getPitch()
    {
        return euler.y() - offset_y;
    }

    double getRoll()
    {
        return euler.z() - offset_z;
    }

    void update()
    {
        euler = bno->getVector(Adafruit_BNO055::VECTOR_EULER);
        linaccel = bno->getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
    }
};
#endif