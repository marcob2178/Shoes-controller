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

1) disconnecting sometimes accelerometer at the shoe
2) walking requires more sensible behavior, we have no code problem, more understanding about the what do we need to get.
3) left back weight sensor requires tuning, we'll tune that
4) crouch require assembling and implementing(guess we can try this flexible things, I have an idea how to reliable detect the crouch)
5) lateral pressure sensors work only if pressing the sensors whe shoe is flat. If i bend sideway to put pressure on the sensors, the lat moivements do not work (will check)
6) button cause joystick drift. (not sure, but we can try. Maybe that's not fixable by hardware)

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
void printTheMovement();
void updateJoysticks();

Joystick leftJoystick;
Joystick rightJoystick; //jump and crouch
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

  leftJoystick.begin(POT_0_CS, LEFT_BUTTON_PIN);
  rightJoystick.begin(POT_1_CS, RIGHT_BUTTON_PIN);
  leftJoystick.setCalibrationData(3, 252, 128,
                                  3, 252, 128);
  rightJoystick.setCalibrationData(HORIZONT_MIN, HORIZONT_MAX, HORIZONT_MIDDLE,
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
}

long long timer = 0;
void loop()
{
  if (millis() > timer)
  {
    timer = millis() + 33;
    updateRawData();

    parseSerial();
  
    printTheMovement();
    translateTheMovement();

    long time = timer - millis();
    //Serial.println(String(time));
  }
}

//   printAccelerationOffset();
//   calculate();

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
#define MOVEMENT_TRANSLATING_OUTPUT 7

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
    if (mess == 't')
      currentOutput = MOVEMENT_TRANSLATING_OUTPUT;
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
//  Movement Translating
//=====================================================================

/*
the boneworks has:
1) Moving with left joystick/left controller in all direction to walk (no press of the joystick button)
2) Moving with left joystick/left controller in all direction while keeping pressed the left joystick button . This is to run
3) jump = is the press of the right joystick
4) crouch = is the moving of the right joystick down

*/

int left_x, left_y;
int left_button_state;
int right_x, right_y;
int right_button_state;

void translateBending()
{
  //Serial.print("body angle is:\t" + String(chestAccel->getPitch()) + "\t");
  //Serial.print(String(chestAccel->getRoll()) + "\t");

  // //calculating of forward-backward/vertical movement
  // if (chestAccel->getRoll() < 0)
  // {
  //   coor_x = map(chestAccel->getRoll(), 0, CHEST_BACKWARD_MAX, 0, -100);
  // }
  // else
  //   coor_x = map(chestAccel->getRoll(), 0, -CHEST_FORWARD_MAX, 0, 100);

  // if (coor_x > 100)
  //   coor_x = 100;
  // if (coor_x < -100)
  //   coor_x = -100;

  // //calculating of right-left/horizontal movement
  // if (chestAccel->getPitch() < 0)
  //   coor_y = map(chestAccel->getPitch(), 0, CHEST_RIGHT_MAX, 0, 100);
  // else
  //   coor_y = map(chestAccel->getPitch(), 0, -CHEST_LEFT_MAX, 0, -100);

  // if (coor_y > 100)
  //   coor_y = 100;
  // if (coor_y < -100)
  //   coor_y = -100;

  // coor_x = chest->getBendingPower() * 3 * cos(chest->getBendingDirection() / 57.297469);
  // coor_y = chest->getBendingPower() * 3 * sin(chest->getBendingDirection() / 57.297469);

  // if (coor_x > 100)
  //   coor_x = 100;
  // if (coor_x < -100)
  //   coor_x = -100;

  // if (coor_y > 100)
  //   coor_y = 100;
  // if (coor_y < -100)
  //   coor_y = -100;

  // if (coor_x >= CHEST_FORWARD_MIN)
  //   joystickForMove.setVer(coor_x);
  // else if (coor_x <= -CHEST_BACKWARD_MIN)
  //   joystickForMove.setVer(coor_x);
  // else
  //   joystickForMove.setVer(0);

  // if (coor_y >= CHEST_LEFT_MIN)
  //   joystickForMove.setHor(coor_y);
  // else if (coor_y <= -CHEST_RIGHT_MIN)
  //   joystickForMove.setHor(coor_y);
  // else
  //   joystickForMove.setHor(0);

  // Serial.print(String(coor_x) + "\t");
  // Serial.println(String(coor_y) + "\t");
}

int delay_value = 0;

void translateTheMovement()
{
  bool isRightFootWalking = rightFoot->isWalking();
  bool isLeftFootWalking = leftFoot->isWalking();

  bool xchanged = false;
  bool ychanged = false;

  //walking
  if (isRightFootWalking || isLeftFootWalking)
  {
    if (isRightFootWalking)
      left_y = Foot::mapDouble(rightFoot->getWalkingPower(), 0, 2, 0, 100);
    else
      left_y = Foot::mapDouble(leftFoot->getWalkingPower(), 0, 2, 0, 100);
    ychanged = true;
  }

  //bending control
  else if (chest->isBending())
  {
    delay_value = 0;
    if (chestAccel->getRoll() < 0)
      left_y = map(chestAccel->getRoll(), 0, CHEST_BACKWARD_MAX, 0, -100);
    else
      left_y = map(chestAccel->getRoll(), 0, -CHEST_FORWARD_MAX, 0, 100);

    //calculating of right-left/horizontal movement
    if (chestAccel->getPitch() < 0)
      left_x = map(chestAccel->getPitch(), 0, CHEST_RIGHT_MAX, 0, 100);
    else
      left_x = map(chestAccel->getPitch(), 0, -CHEST_LEFT_MAX, 0, -100);
    xchanged = true;
    ychanged = true;
  }

  //cruise control
  if (rightFoot->isCruiseControl())
  {
    left_y = map(rightFoot->getCruiseControlPower(), 10, 25, 25, 100);
    ychanged = true;
  }

  if (leftFoot->isCruiseControl())
  {
    left_y = map(leftFoot->getCruiseControlPower(), 10, 25, 25, 100);
    ychanged = true;
  }

  //side moving
  if (rightFoot->isSideStep())
  {
    left_x = map(rightFoot->getSidePower(), 200, 950, 0, 100);
    xchanged = true;
  }

  if (leftFoot->isSideStep())
  {
    left_x = -map(leftFoot->getSidePower(), 200, 950, 0, 100);
    xchanged = true;
  }

  //back moving

  else if (rightFoot->isStepBack())
  {
    left_y = -map(rightFoot->getStepBackPower(), 200, 850, 0, 100);
    ychanged = true;
  }

  else if (leftFoot->isStepBack())
  {
    left_y = -map(leftFoot->getStepBackPower(), 200, 850, 0, 100);
    ychanged = true;
  }

  //default for moving left joystick

  if (!xchanged)
    left_x = 0;
  if (!ychanged)
    left_y = 0;

    
    right_x = 0;
    right_y = 0;

  //if running very fast - press the button
  if ((abs(left_x) >= 110) || (abs(left_y) >= 110))
    left_button_state = 1;
  else
    left_button_state = 0;

  //jump
  if (chest->isJumping())
    right_button_state = 1;
  else
    right_button_state = 0;

  updateJoysticks();

  if (currentOutput == MOVEMENT_TRANSLATING_OUTPUT)
  {
    Serial.print("Left joystick:\t" + String(left_x) + "\t" + String(left_y) + "\t" + "button\t" + String(left_button_state) + "\t");
    Serial.println("Right joystick:\t" + String(right_x) + "\t" + String(right_y) + "\t" + "button\t" + String(right_button_state) + "\t");
  }
}

void updateJoysticks()
{
  if (left_x > 100)
    left_x = 100;
  if (left_x < -100)
    left_x = -100;

  if (left_y > 100)
    left_y = 100;
  if (left_y < -100)
    left_y = -100;

  if (right_x > 100)
    right_x = 100;
  if (right_x < -100)
    right_x = -100;

  if (right_y > 100)
    right_y = 100;
  if (right_y < -100)
    right_y = -100;

  leftJoystick.setHor(-left_x);
  leftJoystick.setVer(-left_y);

  if (left_button_state == 1)
    leftJoystick.pressButton();
  else
    leftJoystick.releaseButton();

  rightJoystick.setHor(right_x);
  rightJoystick.setVer(right_y);

  if (right_button_state == 1)
    rightJoystick.pressButton();
  else
    rightJoystick.releaseButton();
}

//=====================================================================
//  Prints
//=====================================================================

void printTheMovement()
{
  if (currentOutput == MOVEMENT_CHEST_OUTPUT)
  {
    //jump output
    Serial.print("\tJump:" + String(chest->isJumping() ? "\tDetected" : "\tNothing") + String("\t") + String(chest->isJumping() ? String(chest->getJumpingPower()) : "\t") + String("\t"));

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
    }

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