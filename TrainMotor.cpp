/*
  Motor.cpp
  Created by Alan T. Grier, 23 September 2019.
*/

#include "Arduino.h"
#include "Motor.h"

Lighting::Lighting(int LightPin)
{
  _pin = LightPin;
  pinMode(_pin, OUTPUT);
  Off();
}

void Lighting::Off()
{
  digitalWrite(_pin, LOW);
}

void Lighting::On()
{
  digitalWrite(_pin, HIGH);
}


void _rampSpeed(int pin, int start, int target, bool inverse = false)
{
  if (start == target)
  {
    return;
  }
  int speed = start;
  int dir = (target - start) / abs(target - start);
  int move_amt = 4;
  int delay_amt = 8;
  if (!inverse)
  {
    analogWrite(pin, speed);
  }
  else
  {
    analogWrite(pin, 255 - speed);
  }
  while (speed != target)
  {
    speed = speed + (move_amt * dir);
    if (dir == 1 && speed > target)
    {
      speed = target;
    }
    else if (dir == -1 && speed < target)
    {
      speed = target;
    }
    if (speed == 0)
      if (!inverse)
      {
        digitalWrite(pin, LOW);
      }
      else
      {
        digitalWrite(pin, HIGH);
      }
    else
    {
      if (!inverse)
      {
        analogWrite(pin, speed);
      }
      else
      {
        analogWrite(pin, 255 - speed);
      }
    }
    delay(delay_amt);
  }
}

SingleMotor::SingleMotor(int speedPin, int motorPin)
{
  pinMode(speedPin, OUTPUT);
  pinMode(motorPin, OUTPUT);
  _speedPin = speedPin;
  _motorPin = motorPin;
  _motorSpeed = 0;
  digitalWrite(_motorPin, LOW);
  analogWrite(_speedPin, _motorSpeed);
  _isEnabled = false;
}

void SingleMotor::setSpeed(int motorSpeed)
{
  if (motorSpeed == _motorSpeed)
    return;
  analogWrite(_speedPin, motorSpeed);
  _motorSpeed = motorSpeed;
}

void SingleMotor::disable()
{
  if (!_isEnabled)
    return;
  digitalWrite(_motorPin, LOW);
  _isEnabled = false;
}

void SingleMotor::enable()
{
  if (_isEnabled)
    return;
  digitalWrite(_motorPin, HIGH);
  _isEnabled = true;
}

DualMotor::DualMotor(int speedPin, int motor1Pin, int motor2Pin)
{
  pinMode(speedPin, OUTPUT);
  pinMode(motor1Pin, OUTPUT);
  pinMode(motor2Pin, OUTPUT);
  _speedPin = speedPin;
  _motor1Pin = motor1Pin;
  _motor2Pin = motor2Pin;
  _motorSpeed = 0;
  _motorDirection = 1;
  digitalWrite(_motor1Pin, LOW);
  digitalWrite(_motor2Pin, LOW);
  analogWrite(_speedPin, _motorSpeed);
  _isEnabled = false;
}

void DualMotor::setSpeed(int motorSpeed)
{
  int _msp = abs(motorSpeed);
  int _dir = (motorSpeed < 0) ? -1 : 1;
  if (_msp == _motorSpeed && _dir == _motorDirection)
    return;
  if (_msp != _motorSpeed && _dir == _motorDirection)
  {
    _rampSpeed(_speedPin, _motorSpeed, _msp);
    _motorSpeed = _msp;
    _isEnabled = true;
    return;
  }
  _rampSpeed(_speedPin, _motorSpeed, 0);
  if (_motorDirection == 1)
  {
    digitalWrite(_motor2Pin, LOW);
    digitalWrite(_motor1Pin, HIGH);
  }
  else
  {
    digitalWrite(_motor1Pin, LOW);
    digitalWrite(_motor2Pin, HIGH);
  }
  _rampSpeed(_speedPin, 0, _msp);
  _motorSpeed = _msp;
  _isEnabled = true;
}

void DualMotor::disable()
{
  if (!_isEnabled)
    return;
  digitalWrite(_motor1Pin, LOW);
  digitalWrite(_motor2Pin, LOW);
  _isEnabled = false;
}

void DualMotor::enable()
{
  if (_isEnabled)
    return;
  if (_motorDirection == 1)
  {
    digitalWrite(_motor2Pin, LOW);
    digitalWrite(_motor1Pin, HIGH);
  }
  else
  {
    digitalWrite(_motor1Pin, LOW);
    digitalWrite(_motor2Pin, HIGH);
  }
  _isEnabled = true;
}


TwoPinMotor::TwoPinMotor(int motor1Pin, int motor2Pin, Lighting* front = 0, Lighting* rear = 0)
{
  pinMode(motor1Pin, OUTPUT);
  pinMode(motor2Pin, OUTPUT);
  _motor1Pin = motor1Pin;
  _motor2Pin = motor2Pin;
  _motorSpeed = 0;
  _motorDirection = _motor1Pin;
  digitalWrite(_motor1Pin, HIGH);
  digitalWrite(_motor2Pin, HIGH);
  _isEnabled = false;
  if ((front == 0 && rear != 0) || (front != 0 && rear == 0))
  {
    front = 0;
    rear = 0;
  }
  _front = front;
  _rear = rear;
}

void TwoPinMotor::setSpeed(int motorSpeed)
{
  int msp = abs(motorSpeed);
  int dir = (motorSpeed < 0) ? _motor2Pin : _motor1Pin;
  if (msp == _motorSpeed && dir == _motorDirection)
    return;
  if (_front != NULL)
  {
    if (dir == _motor2Pin)
    {
      _rear->On();
      _front->Off();
    }
    else
    {
      _front->On();
      _rear->Off();
    }
  }
  if (msp != _motorSpeed && dir == _motorDirection)
  {
    _rampSpeed(_motorDirection, _motorSpeed, msp, true);
    _motorSpeed = msp;
    _isEnabled = true;
    return;
  }
  _rampSpeed(_motorDirection, _motorSpeed, 0, true);
  _motorSpeed = msp;
  _motorDirection = dir;
  if (_front != NULL)
  {
    if (dir == _motor2Pin)
    {
      _rear->On();
      _front->Off();
    }
    else
    {
      _front->On();
      _rear->Off();
    }
  }
  _rampSpeed(_motorDirection, 0, _motorSpeed, true);
  _isEnabled = true;
}

void TwoPinMotor::disable(bool zeroSpeed=false)
{
  if (zeroSpeed)
    setSpeed(0);
  if (!_isEnabled)
    return;
  digitalWrite(_motor1Pin, HIGH);
  digitalWrite(_motor2Pin, HIGH);
  _isEnabled = false;
}

void TwoPinMotor::enable()
{
  if (_isEnabled)
    return;
  digitalWrite(_motor1Pin + _motor2Pin - _motorDirection, HIGH);
  analogWrite(_motorDirection, 255 - _motorSpeed);
  _isEnabled = true;
}
