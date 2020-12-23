/*
  receiver.ino
  Created by Alan T. Grier, 23 September 2019.
*/

#include "receiver.h"

void setup()
{
    Serial.begin(115200); // configure serial interface
    Serial.flush();

    // Initialize radio
    manager.init();
    driver.setFrequency(RF69_FREQ);
    driver.setTxPower(20, true);
    uint8_t key[] = {0xa, 0xb, 0xa, 0xd, 0xc, 0xa, 0xf, 0xe,
                     0xd, 0xe, 0xa, 0xd, 0xb, 0xe, 0xe, 0xf};
    driver.setEncryptionKey(key);
    manager.setTimeout(50);

    pinMode(PIN_TRACK, INPUT);
    // pinMode(13, OUTPUT);
    // digitalWrite(13, HIGH);
    cab_light.On();
}

void loop()
{
    readTrackVoltage();

    if (manager.available())
    {
        // Serial.println("manager");
        uint8_t len = sizeof(buf);
        uint8_t from;
        if (manager.recvfrom(buf, &len, &from))
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

void readTrackVoltage()
{
    if (analogRead(PIN_TRACK) < BATTERY_THRESHOLD)
        shutdown();
}

void shutdown()
{
    locomotive.disable(true);
    cab_light.Off();
    noInterrupts();
    while (1)
        ;
}

void throttle(uint8_t *command)
{
    int spd = command[1];
    int dir = command[2];
    //Serial.print(spd);
    //Serial.println(dir);
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
