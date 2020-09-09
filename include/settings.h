#ifndef SETTINGS_H
#define SETTINGS_H

//========================================================================
//  Pinout
//========================================================================

#define LEFT_BUTTON_PIN 4
#define RIGHT_BUTTON_PIN 7
#define RIGHT_ACCEL_TWI_ADRESS 0x28
#define LEFT_ACCEL_TWI_ADRESS 0x29
#define POT_0_CS 6
#define POT_1_CS 5
#define Sensor_BR A1
#define Sensor_SR A3
#define Sensor_BL A0
#define Sensor_SL A2

//========================================================================
//  JOYSTICK
//========================================================================

//calibration values
#define HORIZONT_MIN 36
#define HORIZONT_MAX 243
#define HORIZONT_MIDDLE 127
#define VERTICAL_MIN 10
#define VERTICAL_MAX 243
#define VERTICAL_MIDDLE 127

//========================================================================
//  THRESHOLDS for moving recognition 
//========================================================================

#define CALCULATING_PERIOD 10
#define FEET_ANGLE 10

//no negative values, use absolute
#define CHEST_FORWARD_MIN 5
#define CHEST_FORWARD_MAX 25

#define CHEST_BACKWARD_MIN 5
#define CHEST_BACKWARD_MAX 25

#define CHEST_LEFT_MIN 5
#define CHEST_LEFT_MAX 25

#define CHEST_RIGHT_MIN 5
#define CHEST_RIGHT_MAX 25

#endif