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

TwoPinMotor TwoPinMotor(-1, -1, -1, -1);

// // create list of registers for MAX_MAIN_REGISTER Main Track Packets
// volatile RegisterList mainRegs(1);
// uint8_t *command;

// // DCC output pins
// #define DCC_1 9
// #define DCC_2 10

// // Timer duration and interrupt values for 0/1 bits
// #define DCC_ZERO_BIT_TOTAL_DURATION 199
// #define DCC_ZERO_BIT_PULSE_DURATION 99
// #define DCC_ONE_BIT_TOTAL_DURATION 115
// #define DCC_ONE_BIT_PULSE_DURATION 57

void setup()
{
    // SerialCommand::init(&mainRegs); // create structure to read and parse commands from serial line

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

    // // Set DCC pins out output
    // pinMode(DCC_1, OUTPUT);
    // pinMode(DCC_2, OUTPUT);

    // // 32u4 Timer 4, 10 bit
    // // Enable Timer 4B output
    // bitSet(DDRB, 6);

    // // Set waveform mode to up/reset
    // bitClear(TCCR4D, WGM40);
    // bitClear(TCCR4D, WGM41);

    // // Set prescale to /8
    // bitClear(TCCR4B, CS40);
    // bitClear(TCCR4B, CS41);
    // bitSet(TCCR4B, CS42);
    // bitClear(TCCR4B, CS43);

    // // Set comparator to toggle/inverted
    // bitSet(TCCR4A, PWM4B);
    // bitSet(TCCR4A, COM4B0);
    // bitClear(TCCR4A, COM4B1);

    // // Set one bits as default write
    // OCR4C = DCC_ONE_BIT_TOTAL_DURATION;
    // OCR4B = DCC_ONE_BIT_PULSE_DURATION;

    // // load idle packet into register 1
    // mainRegs.loadPacket(1, RegisterList::idlePacket, 2, 0);

    // // Enable interrupt vector
    // bitSet(TIMSK4, OCIE4B);

    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
}

void loop()
{
    // command = 0;
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
            // command = buf;
            // SerialCommand::process(command); // check for, and process, and new serial commands
        }
    }
}

// #define DCC_SIGNAL                                                                                                                                                                          \
//     if (mainRegs.currentBit == mainRegs.currentReg->activePacket->nBits)                                                                                                                    \
//     {                            /* IF no more bits in this DCC Packet */                                                                                                                   \
//         mainRegs.currentBit = 0; /* reset current bit pointer and determine which Register and Packet to process next--- */                                                                 \
//                                                                                                                                                                                             \
//         if (mainRegs.nRepeat > 0 && mainRegs.currentReg == mainRegs.reg)                                                                                                                    \
//         {                       /* IF current Register is first Register AND should be repeated */                                                                                          \
//             mainRegs.nRepeat--; /* decrement repeat count; result is this same Packet will be repeated */                                                                                   \
//         }                                                                                                                                                                                   \
//         else if (mainRegs.nextReg != NULL)                                                                                                                                                  \
//         {                                                            /* ELSE IF another Register has been updated */                                                                        \
//             mainRegs.currentReg = mainRegs.nextReg;                  /* update currentReg to nextReg */                                                                                     \
//             mainRegs.nextReg = NULL;                                 /* reset nextReg to NULL */                                                                                            \
//             mainRegs.tempPacket = mainRegs.currentReg->activePacket; /* flip active and update Packets */                                                                                   \
//                                                                                                                                                                                             \
//             mainRegs.currentReg->activePacket = mainRegs.currentReg->updatePacket;                                                                                                          \
//             mainRegs.currentReg->updatePacket = mainRegs.tempPacket;                                                                                                                        \
//         }                                                                                                                                                                                   \
//         else                                                                                                                                                                                \
//         {                                                     /* ELSE simply move to next Register */                                                                                       \
//             if (mainRegs.currentReg == mainRegs.maxLoadedReg) /* BUT IF this is last Register loaded */                                                                                     \
//                 mainRegs.currentReg = mainRegs.reg;           /* first reset currentReg to base Register, THEN */                                                                           \
//             mainRegs.currentReg++;                            /* increment current Register (note this logic causes Register[0] to be skipped when simply cycling through all Registers) */ \
//         }                                                                                                                                                                                   \
//     } /* END-IF: currentReg, activePacket, and currentBit should now be properly set to point to next DCC bit */                                                                            \
//                                                                                                                                                                                             \
//     if (mainRegs.currentReg->activePacket->buf[mainRegs.currentBit / 8] & mainRegs.bitMask[mainRegs.currentBit % 8])                                                                        \
//     {                                       /* IF bit is a ONE */                                                                                                                           \
//         OCR4C = DCC_ONE_BIT_TOTAL_DURATION; /* set OCRA for timer N to full cycle duration of DCC ONE bit */                                                                                \
//         OCR4B = DCC_ONE_BIT_PULSE_DURATION; /* set OCRB for timer N to half cycle duration of DCC ONE but */                                                                                \
//     }                                                                                                                                                                                       \
//     else                                                                                                                                                                                    \
//     {                                        /* ELSE it is a ZERO */                                                                                                                        \
//         OCR4C = DCC_ZERO_BIT_TOTAL_DURATION; /* set OCRA for timer N to full cycle duration of DCC ZERO bit */                                                                              \
//         OCR4B = DCC_ZERO_BIT_PULSE_DURATION; /* set OCRB for timer N to half cycle duration of DCC ZERO bit */                                                                              \
//     }                                        /* END-ELSE */                                                                                                                                 \
//                                                                                                                                                                                             \
//     mainRegs.currentBit++; /* point to next bit in current Packet */

// ///////////////////////////////////////////////////////////////////////////////

// // NOW USE THE ABOVE MACRO TO CREATE THE CODE FOR EACH INTERRUPT

// ISR(TIMER4_COMPB_vect)
// { // set interrupt service for OCR1B of TIMER-1 which flips direction bit of Motor Shield Channel A controlling Main Track
//     DCC_SIGNAL
// }
