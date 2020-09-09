#include "Accelerometer.h"

/** @brief Initializer
 *  @param id  set the device ID I2C
 *  @param address set the device address I2C
 */
Accelerometer::Accelerometer(int id, int address) {
  bno = new Adafruit_BNO055(id, address);
};

/** @brief Calibrates the sernsor */
void Accelerometer::calibrate() {
  euler = bno->getVector(Adafruit_BNO055::VECTOR_EULER);
  setOffsets(euler.y(), euler.z());
}

/** @brief Check Acc Communication */
void Accelerometer::begin() {
  if (!bno->begin()) {
    Serial.print("Ooops, no BNO055 1 detected ... Check your wiring or I2C ADDR!");
    return;
    while (1)
      ;
  }
  //bno->setExtCrystalUse(false);
  delay(500);

  calibrate();
}

/** @brief Gets Linear Acceleration
 *  @returns a Vector3 formated Linear Acceleration
 */
imu::Vector<3> Accelerometer::getLinAccel() {
  return linaccel;
}

/** @brief Get the X axis from the euler Vector
 *  @returns [double]
 */
double Accelerometer::getYaw() {
  return euler.x();
}

/** @brief Get the Y axis from the euler Vector
 *  @returns [double]
 */
double Accelerometer::getPitch() {
  return euler.y() - offset_y;
}

/** @brief Get the Z axis from the euler Vector
 *  @returns [double]
 */
double Accelerometer::getRoll() {
  return euler.z() - offset_z;
}

/** @brief Upadte Vectors */
void Accelerometer::update() {
  euler = bno->getVector(Adafruit_BNO055::VECTOR_EULER);
  //delayMicroseconds(1000);
  linaccel = bno->getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  //delayMicroseconds(1000);
}