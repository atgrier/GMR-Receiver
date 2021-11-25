/*
  receiver.ino
  Created by Alan T. Grier, 23 September 2019.
*/

#include "receiver.h"

void setup()
{
  // Initialize radio
  Serial.begin(115200);
  radio.init(RF69_FREQ);

  // Miscellaneous initialization
  timer_disable = millis();
  pinMode(PIN_BATTERY, INPUT);
}

bool lowBattery()
{
  return (analogRead(PIN_BATTERY) < BATTERY_THRESHOLD);
}

void loop()
{
  if (lowBattery())
    locomotive.shutDown(true);

  if (locomotive.parseRadio())
    timer_disable = millis();

  if (millis() - timer_disable > 500)
    locomotive.shutDown(false);
}
