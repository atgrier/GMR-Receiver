/*
  receiver.ino
  Created by Alan T. Grier, 23 September 2019.
*/

#include "receiver.h"

void shutdown()
{
  locomotive.disable(true);
  light_cab.Off();
  while (1)
    ;
}

void readBatteryVoltage()
{
  if (analogRead(PIN_BATTERY) < BATTERY_THRESHOLD)
    shutdown();
}

void throttle(uint8_t *command)
{
  int spd = (int)command[1];
  int dir = (int)command[2] == 1 ? 1 : -1;

  // Serial.println("");
  // Serial.print(spd);
  // Serial.print(" ");
  // Serial.println(dir);
  // Serial.println(spd * dir);
  if (spd == -1)
    locomotive.disable(true);
  else
    locomotive.setSpeed(spd * dir);
}

void function(uint8_t *command)
{
  0;
}

void setup()
{
  // Initialize radio
  Serial.begin(115200);
  radio.init(RF69_FREQ, RF69_KEY);

  pinMode(PIN_BATTERY, INPUT);
  // pinMode(13, OUTPUT);
  // digitalWrite(13, HIGH);
  light_cab.On();
}

void loop()
{
  // readBatteryVoltage();

  if (radio.available())
  {
    // Serial.println("manager");
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (radio.receive(buf, &len, &from))
    {
      //Serial.print("got request from : 0x");
      //Serial.print(from, HEX);
      //Serial.print(": ");
      // Serial.println(len);
      // Serial.println((char*)buf);

      switch (buf[0])
      {
      case 'e': // E-Stop
        locomotive.disable(true);
        break;

      case 't': // Throttle
        throttle(buf);
        break;

      case 'f': // Function
        function(buf);
        break;
      }

      timer_disable = millis();
    }
  }

  if (millis() - timer_disable > 500)
    locomotive.disable();
}
