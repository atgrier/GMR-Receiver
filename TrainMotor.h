/*
  Motor.h
  Created by Alan T. Grier, 23 September 2019.
*/
#ifndef Motor_h
#define Motor_h

#include "Arduino.h"

class Lighting
{
public:
  Lighting(int LightPin);
  void On();
  void Off();

private:
  int _pin;
};

class SingleMotor
{
public:
  SingleMotor(int speedPin, int motorPin);
  void setSpeed(int motorSpeed); // speed 0 to 255
  void disable();
  void enable();

private:
  int _speedPin;
  int _motorPin;
  int _motorSpeed;
  bool _isEnabled;
};

class DualMotor
{
public:
  DualMotor(int speedPin, int motor1Pin, int motor2Pin);
  void setSpeed(int motorSpeed); // speed -255 to 255
  void disable();
  void enable();

private:
  int _speedPin;
  int _motor1Pin;
  int _motor2Pin;
  int _motorSpeed;
  int _motorDirection;
  bool _isEnabled;
};

class TwoPinMotor
{
public:
  TwoPinMotor(int motor1Pin, int motor2Pin, Lighting* front = 0, Lighting* rear = 0);
  void setSpeed(int motorSpeed); // speed -255 to 255
  void disable(bool zeroSpeed=false);
  void enable();

private:
  Lighting *_front;
  Lighting *_rear;
  int _motor1Pin;
  int _motor2Pin;
  int _motorSpeed;
  int _motorDirection;
  bool _isEnabled;
};

#endif
