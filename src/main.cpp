#include <Arduino.h>
#include <Settings.h>
#include <Joystick.h>
#include <Accelerometer.h>
#include <ChestController.h>
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

void printRawChest();
void printRawRightShoe();
void printRawLeftShoe();
void updateRawData();
void calculateMovement();
void translateTheMovement();
void parseSerial();

double valX_offcet = 0;
double valY_offcet = 0;
double valZ_offcet = 0;
int coor_x = 0, coor_y = 0;

Joystick joystickForMove;
Joystick joystickForJC; //jump and crouch
ChestController chestAccel;
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

  joystickForMove.begin(POT_0_CS);
  joystickForJC.begin(POT_1_CS);
  joystickForMove.setCalibrationData(HORIZONT_MIN, HORIZONT_MAX, HORIZONT_MIDDLE,
                                     VERTICAL_MIN, VERTICAL_MAX, VERTICAL_MIDDLE);
  joystickForJC.setCalibrationData(HORIZONT_MIN, HORIZONT_MAX, HORIZONT_MIDDLE,
                                   VERTICAL_MIN, VERTICAL_MAX, VERTICAL_MIDDLE);

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
  if (millis() > timer)
  {
    timer = millis() + 33;
    updateRawData();

    parseSerial();

    calculateMovement();
    translateTheMovement();
  }
}

//   printAccelerationOffset();
//   calculate();

//   // if (coor_x >= CHEST_FORWARD_MIN)
//   //   joystick.setVer(coor_x);
//   // else if (coor_x <= -CHES  T_BACKWARD_MIN)
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

//=====================================================================
//  Serial Controller
//=====================================================================

#define NO_OUTPUT 0
#define RIGHT_SHOE_OUTPUT 1
#define LEFT_SHOE_OUTPUT 2
#define CHEST_OUTPUT 3
#define MOVEMENT_OUTPUT 4

int currentOutput = 0;

void parseSerial()
{
  if (Serial.available() > 0)
  {
    char mess = Serial.read();
    if (mess == 'r')
      currentOutput = RIGHT_SHOE_OUTPUT;
    if (mess == 'l')
      currentOutput = LEFT_SHOE_OUTPUT;
    if (mess == 'c') //beautiful
      currentOutput = CHEST_OUTPUT;
    if (mess == 'm')
      currentOutput = MOVEMENT_OUTPUT;
    if (mess == 'n')
      currentOutput = NO_OUTPUT;
  }
}

//=====================================================================
//  RAW data updating
//=====================================================================

void updateRawData()
{
  chestAccel.update();
  rightShoeAccel.update();
  leftShoeAccel.update();

  switch (currentOutput)
  {
  case RIGHT_SHOE_OUTPUT:
    printRawRightShoe();
    break;
  case LEFT_SHOE_OUTPUT:
    printRawLeftShoe();
    break;
  case CHEST_OUTPUT:
    printRawChest();
    break;
  }
}

//=====================================================================
//  Movement calculations
//=====================================================================

void calculateMovement()
{
}

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

// void processBody()
// {
//   Serial.print("\tbody angle is:\t" + String(chestAccel.getPitch()));
//   Serial.print("\t" + String(chestAccel.getRoll()));

//   //calculating of forward-backward/vertical movement
//   if (chestAccel.getRoll() < -180)
//   {
//     int roll = chestAccel.getRoll() + 360;
//     coor_x = map(roll, 0, CHEST_BACKWARD_MAX, 0, -100);
//   }
//   else
//     coor_x = map(chestAccel.getRoll(), 0, -CHEST_FORWARD_MAX, 0, 100);

//   if (coor_x > 100)
//     coor_x = 100;
//   if (coor_x < -100)
//     coor_x = -100;

//   //calculating of right-left/horizontal movement
//   if (chestAccel.getPitch() < -180)
//   {
//     int pitch = chestAccel.getPitch() + 360;
//     coor_y = map(pitch, 0, CHEST_RIGHT_MAX, 0, 100);
//   }
//   else
//     coor_y = map(chestAccel.getPitch(), 0, -CHEST_LEFT_MAX, 0, -100);

//   if (coor_y > 100)
//     coor_y = 100;
//   if (coor_y < -100)
//     coor_y = -100;
// }

// bool isStepDone = false;
// int curStepState = 0, prevStepState = 0, stepsCount = 0;

// double lastTimeCounter = 0;
// long timeCounter = 0;
// double lastMovementCount = 0;
// double movementCount = 0;
// double value = 0;

// double mapDouble(double x, double in_min, double in_max, double out_min, double out_max)
// {
//   return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
// }

// void processSteps()
// {
//
// }

// void calculate()
// {
//   processSteps();
//   processBody();

//   Serial.print("\tx:\t" + String(coor_x));
//   Serial.println("\ty:\t" + String(coor_y));
//   //Serial.print("\tlegs: \t" + String(stepsPower));
// }

//===========================================================
//  crouch logic

bool isCrouch()
{
  return chestAccel.getAltitude() < -0.6;
}

double getCrouchPower()
{
  double val = chestAccel.getAltitude() + 0.6;
  return val > 0 ? val : -val;
}

//===========================================================
//  jump logic

double getJumpingPower()
{
  return chestAccel.getAccelZ() > 0 ? chestAccel.getAccelZ() : 0;
}
double isJumping()
{
  return getJumpingPower() > 5;
}
//===========================================================
//  bend logic

int getBendingDirection()
{
  double y = chestAccel.getPitch();
  double x = chestAccel.getRoll();

  double val = 0;
  if ((x > 0) && (y >= 0))
  {
    val = atan(y / x);
  }

  else if ((x > 0) && (y < 0))
  {
    val = atan(y / x) + 2 * PI;
  }

  else if (x < 0)
  {
    val = atan(y / x) + PI;
  }

  else if ((x == 0) && (y > 0))
  {
    val = PI / 2;
  }

  else if ((x == 0) && (y < 0))
  {
    val = (3 * PI) / 2;
  }

  else if ((x == 0) && (y == 0))
  {
    val = 0;
  }

  return (val * 180.0) / PI;
}

double getBendingPower()
{

  int x = chestAccel.getPitch();
  int y = chestAccel.getRoll();

  return sqrt((x * x) + (y * y));
}

bool isBending()
{
  return getBendingPower() > 5;
}

//===========================================================
//  running logic

bool isStepDone = false;
int curStepState = 0, prevStepState = 0, stepsCount = 0;

long timeCounter = 0;
double movementCount = 0;     

double mapDouble(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

double getWalkingPower()
{
  return (movementCount / (timeCounter / 33.0));
}

bool isWalking()
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

    movementCount = 0;
    timeCounter = 0;
  }
  prevStepState = curStepState;

  return timeCounter > 0;

  //  then we need process the acceleration data througth the "moving average algorithm" for 16 values
  //  after that we measure the acceleration power and calculate step power
}

//=====================================================================
//  Movement Translating
//=====================================================================

void translateTheMovement()
{
  if (currentOutput == MOVEMENT_OUTPUT)
  {
    //jump output
    Serial.print("\tJump:" + String(isJumping() ? "\tDetected" : "\tNothing") + String("\t") + String(isJumping() ? String(getJumpingPower()) : "\t"));

    //bend output
    Serial.print("\tBend:");

    if (!isBending())
      Serial.print("\tStraight\t");
    else
    {
      if ((getBendingDirection() > 315) || (getBendingDirection() <= 45))
        Serial.print("\tbackward");

      else if ((getBendingDirection() > 225) && (getBendingDirection() <= 315))
        Serial.print("\tleft");

      else if ((getBendingDirection() > 135) && (getBendingDirection() <= 225))
        Serial.print("\tforward");

      else if ((getBendingDirection() > 45) && (getBendingDirection() <= 135))
        Serial.print("\tright");

      Serial.print(String("\t") + getBendingDirection());
      Serial.print(String("\t") + getBendingPower());
    }

    //crouch output

    // Serial.print("\tCrouch:");
    // if (isCrouch())
    // {
    //   Serial.print("\tDetected" + String("\t") + String(getCrouchPower()));
    // }
    // else
    //   Serial.print("\tNothing");

    //running output

    Serial.print("\tWalk:");
    if (isWalking())
    {
      Serial.print("\tDetected" + String("\t") + String(getWalkingPower()));
    }
    else
      Serial.print("\tNothing");

    Serial.println();
  }
}

// void sendCommandMoving(int direction, int power)
// {
//   switch (direction)
//   {
//   case DIRECTION_RIGHT:
//     //joystickForMove.setHor();
//     break;
//   }
// }

// //FS(final speed) =( BA(bending angle in percents) * SF(scale factor) ) x WS(walking speed)
// int convertSpeedForJoystick()
// {
//   //need to implement some calculations
//   getWalkingSpeed();
// }

//=====================================================================
//  Prints
//=====================================================================

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

void printRawChest()
{

  Serial.print("chest:");
  Serial.print("\tp\t" + String(chestAccel.getPitch()));
  Serial.print("\tr\t" + String(chestAccel.getRoll()));
  Serial.print("\taZ\t" + String(chestAccel.getAccelZ()));
  Serial.println("\talt\t" + String(chestAccel.getAltitude()));
}

void printRawRightShoe()
{
  Serial.print("rshoe:");
  Serial.print("\tax\t" + String(rightShoeAccel.getLinAccel().x()));
  Serial.print("\tay\t" + String(rightShoeAccel.getLinAccel().y()));
  Serial.print("\taz\t" + String(rightShoeAccel.getLinAccel().z()));

  Serial.print("\ty\t" + String(rightShoeAccel.getYaw()));
  Serial.print("\tr\t" + String(rightShoeAccel.getRoll()));
  Serial.print("\tp\t" + String(rightShoeAccel.getPitch()));

  Serial.print("\tweight:");
  Serial.print("\ts\t" + String(rightSideFoot.readRaw()));
  Serial.println("\tb\t" + String(rightBackFoot.readRaw()));
}

void printRawLeftShoe()
{
  Serial.print("lshoe:");
  Serial.print("\tax\t" + String(leftShoeAccel.getLinAccel().x()));
  Serial.print("\tay\t" + String(leftShoeAccel.getLinAccel().y()));
  Serial.print("\taz\t" + String(leftShoeAccel.getLinAccel().z()));

  Serial.print("\ty\t" + String(leftShoeAccel.getYaw()));
  Serial.print("\tr\t" + String(leftShoeAccel.getRoll()));
  Serial.print("\tp\t" + String(leftShoeAccel.getPitch()));

  Serial.print("\tweight:");
  Serial.print("\ts\t" + String(leftSideFoot.readRaw()));
  Serial.println("\tb\t" + String(leftBackFoot.readRaw()));
}

void printRawValues()
{
  Serial.println("(\\/)*===*(\\/)");
  printRawChest();
  printRawRightShoe();
  printRawLeftShoe();
}