/*
  _receiver.h
  Created by Alan T. Grier, 23 September 2019.
*/

#include <Arduino.h>
#include <RHReliableDatagram.h>
#include <RH_RF69.h>
#include <TrainMotor.h>

// Radio parameters
#define CLIENT_ADDRESS 101 // Controller's address
#define SERVER_ADDRESS 202 // Locomotive's address
#define RF69_FREQ 868.0
#define RFM69_CS 8
#define RFM69_INT 7

// Pin assignments
#define PIN_TRACK -1 // IO Pin for reading track voltage
#define PIN_LIGHT_FRONT -1
#define PIN_LIGHT_REAR -1
#define PIN_LIGHT_CAB -1
#define PIN_MOTOR1 -1
#define PIN_MOTOR2 -1
#define PIN_FUNC1 -1
#define PIN_FUNC2 -1

// Other assignments
#define TRACK_THRESHOLD -1 // When to disable train due to undervoltage battery

// Radio initialization
RH_RF69 driver(RFM69_CS, RFM69_INT);
RHReliableDatagram manager(driver, SERVER_ADDRESS);
uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];

// Other initialization
Lighting front_light = Lighting(PIN_LIGHT_FRONT);
Lighting rear_light = Lighting(PIN_LIGHT_REAR);
Lighting cab_light = Lighting(PIN_LIGHT_CAB);
TwoPinMotor locomotive = TwoPinMotor(PIN_MOTOR1, PIN_MOTOR2, &front_light, &rear_light);
long timer_disable = millis();
