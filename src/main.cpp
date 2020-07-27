#include <Arduino.h>
#include <Settings.h>
#include <Joystick.h>
#include <Accelerometer.h>
#include <ChestSensors.h>
#include <WeightSensor.h>

#include <Chest.h>
#include <Foot.h>

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
void translateTheMovement();
void parseSerial();

double valX_offcet = 0;
double valY_offcet = 0;
double valZ_offcet = 0;
int coor_x = 0, coor_y = 0;

Joystick joystickForMove;
Joystick joystickForJC; //jump and crouch
ChestSensors *chestAccel;
Accelerometer *rightShoeAccel;
Accelerometer *leftShoeAccel;
WeightSensor *rightSideFoot;
WeightSensor *rightBackFoot;
WeightSensor *leftSideFoot;
WeightSensor *leftBackFoot;

//logic
Chest *chest;
Foot *rightFoot;
Foot *leftFoot;

void setup()
{
  Serial.begin(2000000);

  chestAccel = new ChestSensors();
  rightShoeAccel = new Accelerometer(1, LEFT_ACCEL_TWI_ADRESS);
  leftShoeAccel = new Accelerometer(2, RIGHT_ACCEL_TWI_ADRESS);
  rightSideFoot = new WeightSensor(Sensor_SR);
  rightBackFoot = new WeightSensor(Sensor_BR);
  leftSideFoot = new WeightSensor(Sensor_SL);
  leftBackFoot = new WeightSensor(Sensor_BL);

  chest = new Chest(chestAccel);
  rightFoot = new Foot(rightShoeAccel, rightSideFoot, rightBackFoot);
  leftFoot = new Foot(leftShoeAccel, leftSideFoot, leftBackFoot);

  delay(1000);
  pinMode(LEFT_BUTTON_PIN, OUTPUT);
  pinMode(RIGHT_BUTTON_PIN, OUTPUT);

  joystickForMove.begin(POT_0_CS);
  joystickForJC.begin(POT_1_CS);
  joystickForMove.setCalibrationData(HORIZONT_MIN, HORIZONT_MAX, HORIZONT_MIDDLE,
                                     VERTICAL_MIN, VERTICAL_MAX, VERTICAL_MIDDLE);
  joystickForJC.setCalibrationData(HORIZONT_MIN, HORIZONT_MAX, HORIZONT_MIDDLE,
                                   VERTICAL_MIN, VERTICAL_MAX, VERTICAL_MIDDLE);

  rightShoeAccel->begin();
  leftShoeAccel->begin();

  Serial.println("Calibrating");
  unsigned long timer = millis();
  while (millis() - timer < 3000)
  {
    leftShoeAccel->calibrate();
    rightShoeAccel->calibrate();
    Serial.print(".");
    delay(50);
  }
  Serial.println("\nDone!");
  Serial.println("Program started!");

  valX_offcet = rightShoeAccel->getLinAccel().x();
  valY_offcet = rightShoeAccel->getLinAccel().y();
  valZ_offcet = rightShoeAccel->getLinAccel().z();
}

long long timer = 0;
void loop()
{
  if (millis() > timer)
  {
    timer = millis() + 33;
    updateRawData();

    parseSerial();

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
#define MOVEMENT_CHEST_OUTPUT 4
#define MOVEMENT_RIGHT_OUTPUT 5
#define MOVEMENT_LEFT_OUTPUT 6

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
      currentOutput = MOVEMENT_CHEST_OUTPUT;
    if (mess == '.')
      currentOutput = MOVEMENT_RIGHT_OUTPUT;
    if (mess == ',')
      currentOutput = MOVEMENT_LEFT_OUTPUT;
    if (mess == 'n')
      currentOutput = NO_OUTPUT;
  }
}

//=====================================================================
//  RAW data updating
//=====================================================================

void updateRawData()
{
  chestAccel->update();
  rightShoeAccel->update();
  leftShoeAccel->update();

  rightSideFoot->update();
  rightBackFoot->update();
  leftSideFoot->update();
  leftBackFoot->update();

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

//=====================================================================
//  Movement Translating
//=====================================================================

void translateTheMovement()
{
  if (currentOutput == MOVEMENT_CHEST_OUTPUT)
  {
    //jump output
    Serial.print("\tJump:" + String(chest->isJumping() ? "\tDetected" : "\tNothing") + String("\t") + String(chest->isJumping() ? String(chest->getJumpingPower()) : "\t"));

    //bend output
    Serial.print("Chest\tBend:");

    if (!chest->isBending())
      Serial.print("\tStraight\t");
    else
    {
      if ((chest->getBendingDirection() > 315) || (chest->getBendingDirection() <= 45))
        Serial.print("\tbackward");

      else if ((chest->getBendingDirection() > 225) && (chest->getBendingDirection() <= 315))
        Serial.print("\tleft");

      else if ((chest->getBendingDirection() > 135) && (chest->getBendingDirection() <= 225))
        Serial.print("\tforward");

      else if ((chest->getBendingDirection() > 45) && (chest->getBendingDirection() <= 135))
        Serial.print("\tright");

      Serial.print(String("\t") + chest->getBendingDirection());
      Serial.print(String("\t") + chest->getBendingPower());

      //crouch output F

      // Serial.print("\tCrouch:");
      // if (isCrouch())
      // {
      //   Serial.print("\tDetected" + String("\t") + String(getCrouchPower()));
      // }
      // else
      //   Serial.print("\tNothing");
      Serial.println();
    }
  }

  // right

  if (currentOutput == MOVEMENT_RIGHT_OUTPUT)
  {
    //running output right

    Serial.print("Right foot\tWalk:");
    if (rightFoot->isWalking())
      Serial.print(String("\t") + String(rightFoot->getWalkingPower()));
    else
      Serial.print("\tNothing\t");

    //special things

    Serial.print("\tCruise");
    if (rightFoot->isCruiseControl())
    {
      Serial.print(String("\t") + String(rightFoot->getCruiseControlPower()));
    }
    else
      Serial.print("\tNothing\t");

    //step back

    Serial.print("\tStepback");
    if (rightFoot->isStepBack())
    {
      Serial.print(String("\t") + String(rightFoot->getStepBackPower()));
    }
    else
      Serial.print("\tNothing\t");

    //step side

    Serial.print("\tStepside");
    if (rightFoot->isSideStep())
    {
      Serial.print(String("\t") + String(rightFoot->getSidePower()));
    }
    else
      Serial.print("\tNothing\t");
    Serial.println();
  }

  // left

  if (currentOutput == MOVEMENT_LEFT_OUTPUT)
  {
    //running output right

    Serial.print("Left foot\tWalk:");
    if (leftFoot->isWalking())
      Serial.print(String("\t") + String(leftFoot->getWalkingPower()));

    else
      Serial.print("\tNothing\t");

    //special things

    Serial.print("\tCruise");
    if (leftFoot->isCruiseControl())
      Serial.print(String("\t") + String(leftFoot->getCruiseControlPower()));

    else
      Serial.print("\tNothing\t");

    //step back

    Serial.print("\tStepback");
    if (leftFoot->isStepBack())
    {
      Serial.print(String("\t") + String(leftFoot->getStepBackPower()));
    }
    else
      Serial.print("\tNothing\t");

    //step side

    Serial.print("\tStepside");
    if (leftFoot->isSideStep())
    {
      Serial.print(String("\t") + String(leftFoot->getSidePower()));
    }
    else
      Serial.print("\tNothing\t");
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
  Serial.print("\tp\t" + String(chestAccel->getPitch()));
  Serial.print("\tr\t" + String(chestAccel->getRoll()));
  Serial.print("\taZ\t" + String(chestAccel->getAccelZ()));
  Serial.println("\talt\t" + String(chestAccel->getAltitude()));
}

void printRawRightShoe()
{
  Serial.print("rshoe:");
  Serial.print("\tax\t" + String(rightShoeAccel->getLinAccel().x()));
  Serial.print("\tay\t" + String(rightShoeAccel->getLinAccel().y()));
  Serial.print("\taz\t" + String(rightShoeAccel->getLinAccel().z()));

  Serial.print("\ty\t" + String(rightShoeAccel->getYaw()));
  Serial.print("\tr\t" + String(rightShoeAccel->getRoll()));
  Serial.print("\tp\t" + String(rightShoeAccel->getPitch()));

  Serial.print("\tweight:");
  Serial.print("\ts\t" + String(rightSideFoot->readRaw()));
  Serial.println("\tb\t" + String(rightBackFoot->readRaw()));
}

void printRawLeftShoe()
{
  Serial.print("lshoe:");
  Serial.print("\tax\t" + String(leftShoeAccel->getLinAccel().x()));
  Serial.print("\tay\t" + String(leftShoeAccel->getLinAccel().y()));
  Serial.print("\taz\t" + String(leftShoeAccel->getLinAccel().z()));

  Serial.print("\ty\t" + String(leftShoeAccel->getYaw()));
  Serial.print("\tr\t" + String(leftShoeAccel->getRoll()));
  Serial.print("\tp\t" + String(leftShoeAccel->getPitch()));

  Serial.print("\tweight:");
  Serial.print("\ts\t" + String(leftSideFoot->readRaw()));
  Serial.println("\tb\t" + String(leftBackFoot->readRaw()));
}

void printRawValues()
{
  Serial.println("(\\/)*===*(\\/)");
  printRawChest();
  printRawRightShoe();
  printRawLeftShoe();
}