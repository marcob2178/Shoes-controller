#include <Arduino.h>
#include <Settings.h>
#include <Joystick.h>
#include <Accelerometer.h>
#include <ChestAccel.h>
#include <WeightSensor.h> 

/*  
TODO: 
1) Solve the controller problem, add different solutions for different systems (buttons, joystick up-down), could be enabled/ through the code.
2) Add logic for bend and the direction of moving
3) Add side moving with Weight sensors or based on accelerometer

STACK:
- usb injection (https://github.com/TrueOpenVR/TrueOpenVR-Drivers)  
- GUI for smoother configuration of the system
*/

void printAcceleration();
void printRawValues();
void printAccelerationOffset();
void calculate();

double valX_offcet = 0;
double valY_offcet = 0;
double valZ_offcet = 0;
int coor_x = 0, coor_y = 0;

Joystick joystickForMove;
Joystick joystickForJC; //jump and crouch
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
  pinMode(LEFT_BUTTON_PIN, OUTPUT);
  pinMode(RIGHT_BUTTON_PIN, OUTPUT);

  joystickForMove.setCalibrationData(HORIZONT_MIN,
                                     HORIZONT_MAX,
                                     HORIZONT_MIDDLE,
                                     VERTICAL_MIN,
                                     VERTICAL_MAX,
                                     VERTICAL_MIDDLE);
  joystickForMove.begin(POT_0_CS);

  joystickForJC.setCalibrationData(HORIZONT_MIN,
                                   HORIZONT_MAX,
                                   HORIZONT_MIDDLE,
                                   VERTICAL_MIN,
                                   VERTICAL_MAX,
                                   VERTICAL_MIDDLE);
  joystickForJC.begin(POT_1_CS);

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

  valX_offcet = rightShoeAccel.getLinAccel().x();
  valY_offcet = rightShoeAccel.getLinAccel().y();
  valZ_offcet = rightShoeAccel.getLinAccel().z();
}

long long timer = 0;
void loop()
{
    
  if (Serial.available() > 0)
  {
    char mess = Serial.read();
    if (mess == '1')
    {
      Serial.println("left Button High");
      digitalWrite(LEFT_BUTTON_PIN, HIGH);
    }
    if (mess == '2')
    {
      Serial.println("left Button Low");
      digitalWrite(LEFT_BUTTON_PIN, LOW);
    }

    if (mess == '3')
    {
      Serial.println("Right Button High");
      digitalWrite(RIGHT_BUTTON_PIN, HIGH);
    }
    if (mess == '4')
    {
      Serial.println("Right Button Low");
      digitalWrite(RIGHT_BUTTON_PIN, LOW);
    }
  }
  //joystickForMove.doCalibration();
  //joystickForJC.doCalibration();
  // if (millis() > timer)
  // {
  //   timer = millis() + 33;
  //   chestAccel.update();
  //   rightShoeAccel.update();
  //   leftShoeAccel.update();

  //   printAccelerationOffset();
  //   calculate();

  //   // if (coor_x >= CHEST_FORWARD_MIN)
  //   //   joystick.setVer(coor_x);
  //   // else if (coor_x <= -CHEST_BACKWARD_MIN)
  //   //   joystick.setVer(coor_x);
  //   // else
  //   //   joystick.setVer(0);

  //   // if (abs(coor_y) > CHEST_LEFT_MIN)
  //   //   joystick.setHor(coor_y);
  //   // else if (coor_y <= -CHEST_RIGHT_MIN)
  //   //   joystick.setHor(coor_y);
  //   // else
  //   //   joystick.setHor(0);

  //   //Serial.println("\tLoop time = " + String(int(timer - millis())));
  // }
}

//=====================================================================
//  Algorithm
//=====================================================================
/* 

Movement recognition:
- Jump (weight sensord + accelerations)
- Crouch  (acceleration?)
- Walk(shoes acceleration + body's angle)
- Cruise Control (shoes angle)

Detected behavior:
- Usual shoes angles when walk - 0 .. 30 degrees (roll)

small slow: 20-30% and 20ms
huge slow: 20-30% and 500ms
small quick: 70-100% and 20ms
huge quick: 70-100% and 500ms

also, we need to scale body's bend:
like 0-45 deg to 0-100%

*/

void processBody()
{
  Serial.print("\tbody angle is:\t" + String(chestAccel.getPitch()));
  Serial.print("\t" + String(chestAccel.getRoll()));

  //calculating of forward-backward/vertical movement
  if (chestAccel.getRoll() < -180)
  {
    int roll = chestAccel.getRoll() + 360;
    coor_x = map(roll, 0, CHEST_BACKWARD_MAX, 0, -100);
  }
  else
    coor_x = map(chestAccel.getRoll(), 0, -CHEST_FORWARD_MAX, 0, 100);

  if (coor_x > 100)
    coor_x = 100;
  if (coor_x < -100)
    coor_x = -100;

  //calculating of right-left/horizontal movement
  if (chestAccel.getPitch() < -180)
  {
    int pitch = chestAccel.getPitch() + 360;
    coor_y = map(pitch, 0, CHEST_RIGHT_MAX, 0, 100);
  }
  else
    coor_y = map(chestAccel.getPitch(), 0, -CHEST_LEFT_MAX, 0, -100);

  if (coor_y > 100)
    coor_y = 100;
  if (coor_y < -100)
    coor_y = -100;
}

bool isStepDone = false;
int curStepState = 0, prevStepState = 0, stepsCount = 0;

double lastTimeCounter = 0;
long timeCounter = 0;
double lastMovementCount = 0;
double movementCount = 0;
double value = 0;

double mapDouble(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void processSteps()
{
  isStepDone = false;
  curStepState = abs((int)rightShoeAccel.getRoll());

  //  calculate duration of step
  if (curStepState >= FEET_ANGLE)
  {
    timeCounter += 33;
    movementCount += rightShoeAccel.getLinAccel().z() > 0 ? rightShoeAccel.getLinAccel().z() : -rightShoeAccel.getLinAccel().z();
  }

  if ((curStepState < FEET_ANGLE) && (prevStepState >= FEET_ANGLE))
  {
    isStepDone = true;
    stepsCount++;
    lastTimeCounter = timeCounter;
    lastMovementCount = movementCount;

    movementCount = 0;
    timeCounter = 0;
  }

  value = (movementCount / (timeCounter / 33.0));
  if (value > 12.0)
    value = 12.0;
  if (value > 0)
    joystickForMove.setVer(mapDouble(value, 0, 12, 0, 100));
  if (isStepDone)
    joystickForMove.setVer(0);

  prevStepState = curStepState;
  Serial.print("\t" + String());
  Serial.print("\t" + String(stepsCount));
  Serial.print("\t" + String(lastMovementCount));
  Serial.print("\t" + String(value));

  //  then we need process the acceleration data througth the "moving a verage algorithm" for 16 values
  //  after that we measure the acceleration power and calculate step power
}

void calculate()
{
  processSteps();
  processBody();

  Serial.print("\tx:\t" + String(coor_x));
  Serial.println("\ty:\t" + String(coor_y));
  //Serial.print("\tlegs: \t" + String(stepsPower));
}

//=====================================================================
//  Prints
//=====================================================================

void printAngles()
{
  Serial.print("\tright shoe:");

  Serial.print("\tyaw: \t" + String(rightShoeAccel.getYaw()));
  Serial.print("\tpitch: \t" + String(rightShoeAccel.getPitch()));
  Serial.print("\troll: \t" + String(rightShoeAccel.getRoll()));

  Serial.print("\tleft shoe:");

  Serial.print("\tyaw: \t" + String(leftShoeAccel.getYaw()));
  Serial.print("\tpitch: \t" + String(leftShoeAccel.getPitch()));
  Serial.println("\troll: \t" + String(leftShoeAccel.getRoll()));
}

void printAccelerationOffset()
{
  Serial.print("\t" + String(rightShoeAccel.getLinAccel().x() - valX_offcet));
  Serial.print("\t" + String(rightShoeAccel.getLinAccel().y() - valY_offcet));
  Serial.print("\t" + String(rightShoeAccel.getLinAccel().z() - valZ_offcet));
}

void printAcceleration()
{
  Serial.print("\tright shoe:");
  Serial.print("\t" + String((double)(rightShoeAccel.getLinAccel().x())));
  Serial.print("\t" + String((double)(rightShoeAccel.getLinAccel().y())));
  Serial.print("\t" + String((double)(rightShoeAccel.getLinAccel().z())));

  Serial.print("\tleft shoe:");
  Serial.print("\t" + String((double)(leftShoeAccel.getLinAccel().x())));
  Serial.print("\t" + String((double)(leftShoeAccel.getLinAccel().y())));
  Serial.println("\t" + String((double)(leftShoeAccel.getLinAccel().z())));
}

void printRawValues()
{
  Serial.print("\tchest:");
  Serial.print("\t" + String(chestAccel.getPitch()));
  Serial.print("\t" + String(chestAccel.getRoll()));

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