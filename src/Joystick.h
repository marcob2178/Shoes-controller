#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "mcp4261.h"
#include <SPI.h>
#include "settings.h"

//calibratet settings for joystick

class Joystick
{
private:
    MCP4261 *pot0;
    int ver_min, ver_max, ver_middle, hor_min, hor_max, hor_middle;

public:
    void setCalibrationData(int _ver_min, int _ver_max, int _ver_middle, int _hor_min, int _hor_max, int _hor_middle)
    {
        ver_min = _ver_min;
        ver_max = _ver_max;
        ver_middle = _ver_middle;
        hor_min = _hor_min;
        hor_max = _hor_max;
        hor_middle = _hor_middle;
    }

    void setVer(int x)
    {
        if (x > 0)
        {
            pot0->setWiper1(map(x, 0, 100, ver_middle, ver_max));
            //Serial.println(map(x, 0, 100, ver_middle, VERTICAL_MAX));
        }
        else if (x < 0)
        {
            pot0->setWiper1(map(x, 0, -100, ver_middle, ver_min));
            //Serial.println(map(x, 0, -100, ver_middle, ver_min));
        }
        else if (x == 0)
        {
            pot0->setWiper1(ver_middle);
            //Serial.println(ver_middle);
        }
    }

    void setHor(int y)
    {
        if (y > 0)
        {
            pot0->setWiper0(map(y, 0, 100, hor_middle, hor_max));
            //Serial.println(map(y, 0, 100, HORIZONT_MIDDLE, HORIZONT_MAX));
        }
        else if (y < 0)
        {
            pot0->setWiper0(map(y, 0, -100, hor_middle, hor_min));
            //Serial.println(map(y, 0, -100, HORIZONT_MIDDLE, HORIZONT_MIN));
        }
        else if (y == 0)
        {
            pot0->setWiper0(hor_middle);
            //Serial.println(HORIZONT_MIDDLE);
        }
    }

    void begin()
    {
        pot0 = new MCP4261(POT_0_CS);
        // Setup SPI communications
        SPI.setDataMode(SPI_MODE0);
        SPI.setBitOrder(MSBFIRST);
        SPI.setClockDivider(SPI_CLOCK_DIV8);
        SPI.begin();

        // Initialize potentiometers
        pot0->initialize();
    }

    void doCalibration()
    {
        if (Serial.available() > 0)
        {
            char mess = Serial.read();
            if (mess == '1')
            {
                setVer(0);
                setHor(0);
                for (int i = 0; i <= 100; i++)
                {
                    setVer(i);
                    delay(30);
                }
            }
            if (mess == '2')
            {
                setVer(0);
                setHor(0);

                for (int i = 0; i >= -100; i--)
                {
                    setVer(i);
                    delay(30);
                }
            }
            if (mess == '3')
            {
                setVer(0);
                setHor(0);

                for (int i = 0; i <= 100; i++)
                {
                    setHor(i);
                    delay(30);
                }
            }
            if (mess == '4')
            {
                setVer(0);
                setHor(0);
                for (int i = 0; i >= -100; i--)
                {
                    setHor(i);
                    delay(30);
                }
            }
        }
    }
};

#endif