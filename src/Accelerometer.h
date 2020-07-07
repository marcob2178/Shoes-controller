#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <Adafruit_BNO055.h>

class Accelerometer
{
private:
    Adafruit_BNO055 *bno;
    imu::Vector<3> euler;
    imu::Vector<3> linaccel;

    void
    calibrate()
    {
        //unsigned long timer;
        //   Serial.println("Calibration...");
        //   while (millis() - timer < 10000)
        //   {
        //   imu::Vector<3> euler1 = bno1.getVector(Adafruit_BNO055::VECTOR_EULER);
        //     imu::Vector<3> euler2 = bno2.getVector(Adafruit_BNO055::VECTOR_EULER);

        //     y1_thres = euler1.z();
        //     y2_thres = euler2.z();
        //     Serial.print(".");
        //     delay(100);
        //   }
        //   Serial.println("\nDone");
        //   Serial.println(String(y1_thres) + " " + String(y2_thres));
        //   delay(3000);
        // }
    }

public:
    Accelerometer(int address)
    {
        bno = new Adafruit_BNO055(-1, address);
    };

    void begin()
    {
        if (!bno->begin())
        {
            Serial.print("Ooops, no BNO055 1 detected ... Check your wiring or I2C ADDR!");
            while (1)
                ;
        }
        delay(1000);

        bno->setExtCrystalUse(true);
        calibrate();
    }

    imu::Vector<3> getLinAccel()
    {
        return linaccel;
    }

    imu::Vector<3> getEuler()
    {
        return euler;
    }

    void update()
    {
        euler = bno->getVector(Adafruit_BNO055::VECTOR_EULER);
        linaccel = bno->getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
    }
};
#endif