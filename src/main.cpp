#include <Arduino.h>
#include <settings.h>
#include <joystick.h>
#include <Accelerometer.h>
#include <ChestAccel.h>
#include <WeightSensor.h>

/*  

TODO:
- add 2nd digipot for crouch and jump
- define button using for current joystick(it doesn't implemented yet)  
- check bno055 calibration 

STACK:
- usb injection (https://github.com/TrueOpenVR/TrueOpenVR-Drivers)  
*/

/* 

Movement recognition:
- Jump (weight sensord + accelerations)
- Crouch  (acceleration?)
- Walk(shoes acceleration + body's angle)
- Cruise Control (shoes angle)

Detected behavior:
- Usual shoes angles when walk - 0 .. 30 degrees (roll)
*/

void printAcceleration();
void printRawValues();

Joystick joystick;
ChestAccel chestAccel;
Accelerometer rightShoeAccel(LEFT_ACCEL_TWI_ADRESS);
Accelerometer leftShoeAccel(RIGHT_ACCEL_TWI_ADRESS);
WeightSensor rightSideFoot(Sensor_SR);
WeightSensor rightBackFoot(Sensor_BR);
WeightSensor leftSideFoot(Sensor_SL);
WeightSensor leftBackFoot(Sensor_BL);

void setup()
{
  Serial.begin(2000000);

  delay(500);

  joystick.setCalibrationData(HORIZONT_MIN,
                              HORIZONT_MAX,
                              HORIZONT_MIDDLE,
                              VERTICAL_MIN,
                              VERTICAL_MAX,
                              VERTICAL_MIDDLE);
  joystick.begin();
  rightShoeAccel.begin();
  leftShoeAccel.begin();

  Serial.println("Calibrating");
  unsigned long timer = millis();
  while (millis() - timer < 2000)
  {
    leftShoeAccel.calibrate();
    rightShoeAccel.calibrate();
    Serial.print(".");
    delay(100);
  }
  Serial.println("\nDone!");

  Serial.println("Program started!");
}

void loop()
{
  chestAccel.update();
  rightShoeAccel.update();
  leftShoeAccel.update();

  printAcceleration();

  delay(33);
}




























//=====================================================================
//  Prints
//=====================================================================

void printAcceleration()
{
  Serial.print("\tright shoe:");
  // Serial.print("\t" + String(rightShoeAccel.getLinAccel().x()));
  // Serial.print("\t" + String(rightShoeAccel.getLinAccel().y()));
  // Serial.print("\t" + String(rightShoeAccel.getLinAccel().z()));

  Serial.print("\tyaw: \t" + String(rightShoeAccel.getYaw()));
  Serial.print("\tpitch: \t" + String(rightShoeAccel.getPitch()));
  Serial.print("\troll: \t" + String(rightShoeAccel.getRoll()));

  Serial.print("\tleft shoe:");
  // Serial.print("\t" + String(leftShoeAccel.getLinAccel().x()));
  // Serial.print("\t" + String(leftShoeAccel.getLinAccel().y()));
  // Serial.print("\t" + String(leftShoeAccel.getLinAccel().z()));

  Serial.print("\tyaw: \t" + String(leftShoeAccel.getYaw()));
  Serial.print("\tpitch: \t" + String(leftShoeAccel.getPitch()));
  Serial.println("\troll: \t" + String(leftShoeAccel.getRoll()));
}

void printRawValues()
{
  Serial.print("\tchest:");
  Serial.print("\t" + String(chestAccel.getX()));
  Serial.print("\t" + String(chestAccel.getY()));

  Serial.print("\tright shoe:");
  Serial.print("\t" + String(rightShoeAccel.getLinAccel().x()));
  Serial.print("\t" + String(rightShoeAccel.getLinAccel().y()));
  Serial.print("\t" + String(rightShoeAccel.getLinAccel().z()));
  Serial.print("\tweight:");
  Serial.print("\t" + String(rightSideFoot.readRaw()));
  Serial.print("\t" + String(rightBackFoot.readRaw()));

  Serial.print("\tleft shoe:");
  Serial.print("\t" + String(leftShoeAccel.getLinAccel().x()));
  Serial.print("\t" + String(leftShoeAccel.getLinAccel().y()));
  Serial.print("\t" + String(leftShoeAccel.getLinAccel().z()));
  Serial.print("\tweight:");
  Serial.print("\t" + String(leftSideFoot.readRaw()));
  Serial.println("\t" + String(leftBackFoot.readRaw()));
}

// void checkV()
// {
//   /*if (pox == 8)
//     pos = 8;
//     else if (pox == 9)
//     pos = 9;
//     else */

//   if (y3_axis_1 > y3_thres_1 - 15 && y3_axis_1 < y3_thres_1 + 15 && y3_axis_2 > y3_thres_2 - 15 && y3_axis_2 < y3_thres_2 + 15)
//     pos = 0; //center
//   else if (y3_axis_1 > 0)
//   { //RIGHT
//     if (y3_axis_2 > y3_thres_2 - 15 && y3_axis_2 < y3_thres_2 + 15)
//       pos = 5; //RIGHT
//     else if (y3_axis_2 > 0)
//     {
//       if (y3_axis_1 > y3_thres_1 - 15 && y3_axis_1 < y3_thres_1 + 15)
//         pos = 7; //UP
//       else
//         pos = 1; //RIGHT UP
//     }
//     else if (y3_axis_2 < 0)
//     {
//       if (y3_axis_1 > y3_thres_1 - 15 && y3_axis_1 < y3_thres_1 + 15)
//         pos = 8; //DOWN
//       else
//         pos = 2; //RIGHT DOWN
//     }
//   }
//   else if (y3_axis_1 < 0)
//   { //LEFT
//     if (y3_axis_2 > y3_thres_2 - 15 && y3_axis_2 < y3_thres_2 + 15)
//       pos = 6; //LEFT
//     else if (y3_axis_2 > 0)
//     {
//       if (y3_axis_1 > y3_thres_1 - 15 && y3_axis_1 < y3_thres_1 + 15)
//         pos = 7; //UP
//       else
//         pos = 3; //UP LEFT
//     }
//     else if (y3_axis_2 < 0)
//     {
//       if (y3_axis_1 > y3_thres_1 - 15 && y3_axis_1 < y3_thres_1 + 15)
//         pos = 8; //DOWN
//       else
//         pos = 4; //LEFT DOWN
//     }
//   }
// }

// void loop() {
//   imu::Vector<3> euler1 = bno1.getVector(Adafruit_BNO055::VECTOR_EULER);
//   imu::Vector<3> euler2 = bno2.getVector(Adafruit_BNO055::VECTOR_EULER);

//   y1_axis = euler1.z();
//   y2_axis = euler2.z();
//   receiveData();
//   sensorBL = analogRead(Sensor_BL);
//   sensorBR = analogRead(Sensor_BR);

//   sensorSL = analogRead(Sensor_SL);
//   sensorSR = analogRead(Sensor_SR);
//   checkV();

//   if (sensorSL > 120) {
//     speed = map( sensorSL , 0 , 1023 , 0 , 255);
//     if (sensorSL < 700) {
//       //servo2.write(70, speed);
//       pot0.setWiper0(40);
//       press1 = 0;
//     }
//     else {
//       //servo2.write(3 , speed);
//       pot0.setWiper0(0);
//       press1 = 0;
//     }
//   }
//   else if (sensorSR > 120) {
//     speed = map( sensorSR , 0 , 1023 , 0 , 255);

//     if (sensorSR < 700) {
//       //servo2.write(110, speed);
//       pot0.setWiper0(220);
//       press1 = 0;
//     }
//     else {
//       //servo2.write(177 , speed);
//       pot0.setWiper0(256);
//       press1 = 0;
//     }
//   }

//   if (sensorSL < 120 && sensorSR < 120 && pos == 0)  {
//     //servo2.write(90 , 255);
//     pot0.setWiper0(127);
//     press1 = 0;
//   }
//   else if (sensorSL < 120 && sensorSR < 120 && pos == 1) {  //left down
//     pot0.setWiper0(0);
//     pot0.setWiper1(0);
//   }
//   else if (sensorSL < 120 && sensorSR < 120 && pos == 2) {  //left up
//     pot0.setWiper0(0);
//     pot0.setWiper1(256);
//   }
//   else if (sensorSL < 120 && sensorSR < 120 && pos == 3) {  //right down
//     pot0.setWiper0(256);
//     pot0.setWiper1(0);
//   }
//   else if (sensorSL < 120 && sensorSR < 120 && pos == 4) {  //right up
//     pot0.setWiper0(256);
//     pot0.setWiper1(256);
//   }
//   else if (sensorSL < 120 && sensorSR < 120 && pos == 5) {  //left
//     pot0.setWiper0(0);
//     pot0.setWiper1(127);
//     press1 = 0;
//   }
//   else if (sensorSL < 120 && sensorSR < 120 && pos == 6) {  //right
//     pot0.setWiper0(256);
//     pot0.setWiper1(127);
//     press1 = 0;
//   }
//   else if (sensorSL < 120 && sensorSR < 120 && pos == 7) {  //down
//     pot0.setWiper0(127);
//     pot0.setWiper1(0);
//     press1 = 0;
//   }
//   else if (sensorSL < 120 && sensorSR < 120 && pos == 8) {  //up
//     pot0.setWiper0(127);
//     pot0.setWiper1(256);
//     press1 = 0;
//   }
//   else if (sensorSL < 120 && sensorSR < 120 && pos == 9) {  //Jump
//     pot0.setWiper0(127);
//     pot0.setWiper1(256);
//     press1 = 0;
//   }
//   else if (sensorSL < 120 && sensorSR < 120 && pos == 10) {  //Crawl
//     pot0.setWiper0(127);
//     pot0.setWiper1(0);
//     press1 = 0;
//   }

//   //0 right  - 64 centr - 127 left

//   if (sensorBL > 250 && pos == 0) {
//     speed = map( sensorBL , 0 , 1023 , 0 , 255);
//     if (sensorBL < 500) { //less pressure 90+20=110
//       //servo1.write(85 , speed);
//       pot0.setWiper1(200);
//       angle = 95;
//       press2 = 0;
//     }
//     else if (sensorBL > 500 && pos == 0) { //more pressre 90+90 = 180
//       //servo1.write(105 , speed);
//       pot0.setWiper1(256);
//       angle = 105;
//       press2 = 0;
//     }
//   }
//   else if (sensorBR > 250 && pos == 0) {
//     speed = map( sensorBR , 0 , 1023 , 0 , 255);
//     if (sensorBR < 500) {  //less pressure
//       //servo1.write(85 , speed);
//       pot0.setWiper1(200);
//       angle = 95;
//       press2 = 0;
//     }
//     else if  (sensorBR > 500 && pos == 0) { //more pressre
//       //servo1.write(105 , speed);
//       pot0.setWiper1(256);
//       angle = 105;
//       press2 = 0;
//     }
//   }
//   //flat dead end no servo movement - center position -  can we separate on different lines the pressure sensor and accell so that I can set the dead end value separately for the press sens and acc ?
//   else if (pos == 0 && (start == 0 || millis() - timer > 800) && y1_axis > y1_thres - SensorV && y1_axis < y1_thres + SensorV && y2_axis > y2_thres - SensorV && y2_axis < y2_thres + SensorV && sensorBL < 200 && sensorBR < 200)
//   {
//     if (angle == 70 && runC > 0) {
//       angle = 78;
//       //servo1.write( 78 , 20);
//       pot0.setWiper1(40);
//       runC--;
//       press2 = 0;
//     }
//     else {
//       //servo1.write( 87 , 15);
//       pot0.setWiper1(128);
//       start = 1;
//       angle = 87;
//       press2 = 0;
//     }
//     //Serial.println("Servo goes to zero");
//     step = 0;
//     steps = "0";
//   }
//   else if ((y1_axis < y1_thres - SensorV) && (step == 0 || step == 2))
//   {
//     step = 1;
//     steps = "1";
//   }
//   else if ((y2_axis < y2_thres - SensorV) && (step == 0 || step == 1))
//   {
//     step = 2;
//     steps = "2";
//   }

//   if (!steps.equals(laststep) && pos == 0) {
//     if ((step == 1 || step == 2)  &&  millis() - timer < 400) {
//       if (freq > 1) {
//         //servo1.write(70 , 20);
//         pot0.setWiper1(0);
//         angle = 70;
//         press2 = 0;
//       }
//       else {
//         //servo1.write(78 , 20);
//         pot0.setWiper1(40);
//         angle = 78;
//         press2 = 0;
//       }
//       freq++;
//       runC = 1;
//     }
//     else if ((step == 1 || step == 2 &&  millis() - timer > 675)) {
//       //servo1.write(78 , 20);
//       pot0.setWiper1(40);
//       angle = 78;
//       freq = 1;
//       press2 = 0;
//     }
//     timer = millis();
//   }

//   if (press1 == 1 || press2 == 1)
//     digitalWrite(Button , LOW);
//   else
//     digitalWrite(Button , HIGH);

//   Serial.print("SL: " + String(sensorSL) + " SR: " + String(sensorSR));
//   Serial.print(" BL: " + String(sensorBL) + " BR: " + String(sensorBR));
//   Serial.print(" Y1: " + String(y1_axis) + " Y2: " + String(y2_axis));
//   Serial.print("Pos: " + String(pos));
//   Serial.print(" Y3x: " + String(y3_axis_1) + " Y3y: " + String(y3_axis_2));
//   Serial.println(" Y3xx: " + String(y3_thres_1) + " Y3yy: " + String(y3_thres_2));

//   laststep = steps;
//   delay(80);
// }
