#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <Arduino.h>
#include <Adafruit_BNO055.h>

class Accelerometer {
  Adafruit_BNO055 *bno;
  imu::Vector<3> euler;
  imu::Vector<3> linaccel;

  double offset_y, offset_z;

  //  x       y       z
  //  yaw     pitch   roll

  void setOffsets(double _offset_y, double _offset_z) {
    offset_y = _offset_y;
    offset_z = _offset_z;
  }

 public:
  /** @details Initialize the Accelerometer. Needs an ID and I2C address. */
  Accelerometer(int id, int address);

  /** @details Calibrates the accelerometer by getting a euler Vector
   * and settings this vector as an offset.
   */
  void calibrate();

  /** @details Check if the Accelerometer is available
   *  $$ This should be implemented in a different way!
   */
  void begin();

  /** @details Public access to lineaccel vector */
  imu::Vector<3> getLinAccel();

  /** @details Public access to the X axis of euler vector */
  double getYaw();

  /** @details Public access to the Y axis of euler vector */
  double getPitch();

  /** @details Public access to the Z axis of euler vector */
  double getRoll();

  /** @details Updates the Vectors */
  void update();
};
#endif