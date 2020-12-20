#include <Arduino.h>
#include <TrainMotor.h>
#include <RHReliableDatagram.h>
#include <RH_RF69.h>

// Radio identifiers
#define CLIENT_ADDRESS 101  // Controller's address
#define SERVER_ADDRESS 202  // Locomotive's address

// Radio initialization
#define RF69_FREQ 868.0
#define RFM69_CS 8
#define RFM69_INT 7
RH_RF69 driver(RFM69_CS, RFM69_INT);
RHReliableDatagram manager(driver, SERVER_ADDRESS);
uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];

// Pin assignments
#define PIN_TRACK -1 // IO Pin for reading track voltage
#define PIN_LIGHT_FRONT -1
#define PIN_LIGHT_REAR -1
#define PIN_LIGHT_CAB -1
#define PIN_MOTOR1 -1
#define PIN_MOTOR2 -1
#define PIN_FUNC1 -1
#define PIN_FUNC2 -1

// Various others
#define TRACK_THRESHOLD -1 // When to disable train due to undervoltage battery

Lighting front_light = Lighting(PIN_LIGHT_FRONT);
Lighting rear_light = Lighting(PIN_LIGHT_REAR);
Lighting cab_light = Lighting(PIN_LIGHT_CAB);
TwoPinMotor locomotive = TwoPinMotor(PIN_MOTOR1, PIN_MOTOR2, &front_light, &rear_light);
long timer_disable = millis();

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
    if (analogRead(PIN_TRACK) < TRACK_THRESHOLD)
        shutdown();

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

            switch(buf[0])
            {
                case 'e' :  // E-Stop
                    locomotive.disable(true);
                    break;

                case 't' :  // Throttle
                    throttle(buf);
                    break;

                case 'f' :  // Function
                    function(buf);
                    break;
            }

            timer_disable = millis();
        }
    }

    if (millis() - timer_disable > 500)
        locomotive.disable();
}

void shutdown()
{
    locomotive.disable(true);
    cab_light.Off();
    noInterrupts();
    while(1) {}
}

void throttle(uint8_t* command)
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

void function(uint8_t* command)
{
    0;
}
