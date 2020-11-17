#include <Arduino.h>
#line 34 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
void setup();
#line 87 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
void loop();
#line 0 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
#line 1 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\Outputs.cpp"
/**********************************************************************

Outputs.cpp
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/
/**********************************************************************

DCC++ BASE STATION supports optional OUTPUT control of any unused Arduino Pins for custom purposes.
Pins can be activited or de-activated.  The default is to set ACTIVE pins HIGH and INACTIVE pins LOW.
However, this default behavior can be inverted for any pin in which case ACTIVE=LOW and INACTIVE=HIGH.

Definitions and state (ACTIVE/INACTIVE) for pins are retained in EEPROM and restored on power-up.
The default is to set each defined pin to active or inactive according to its restored state.
However, the default behavior can be modified so that any pin can be forced to be either active or inactive
upon power-up regardless of its previous state before power-down.

To have this sketch utilize one or more Arduino pins as custom outputs, first define/edit/delete
output definitions using the following variation of the "Z" command:

  <Z ID PIN IFLAG>:            creates a new output ID, with specified PIN and IFLAG values.
                               if output ID already exists, it is updated with specificed PIN and IFLAG.
                               note: output state will be immediately set to ACTIVE/INACTIVE and pin will be set to HIGH/LOW
                               according to IFLAG value specifcied (see below).
                               returns: <O> if successful and <X> if unsuccessful (e.g. out of memory)

  <Z ID>:                      deletes definition of output ID
                               returns: <O> if successful and <X> if unsuccessful (e.g. ID does not exist)

  <Z>:                         lists all defined output pins
                               returns: <Y ID PIN IFLAG STATE> for each defined output pin or <X> if no output pins defined

where

  ID: the numeric ID (0-32767) of the output
  PIN: the arduino pin number to use for the output
  STATE: the state of the output (0=INACTIVE / 1=ACTIVE)
  IFLAG: defines the operational behavior of the output based on bits 0, 1, and 2 as follows:

          IFLAG, bit 0:   0 = forward operation (ACTIVE=HIGH / INACTIVE=LOW)
                          1 = inverted operation (ACTIVE=LOW / INACTIVE=HIGH)

          IFLAG, bit 1:   0 = state of pin restored on power-up to either ACTIVE or INACTIVE depending
                              on state before power-down; state of pin set to INACTIVE when first created
                          1 = state of pin set on power-up, or when first created, to either ACTIVE of INACTIVE
                              depending on IFLAG, bit 2

          IFLAG, bit 2:   0 = state of pin set to INACTIVE uponm power-up or when first created
                          1 = state of pin set to ACTIVE uponm power-up or when first created 
                
Once all outputs have been properly defined, use the <E> command to store their definitions to EEPROM.
If you later make edits/additions/deletions to the output definitions, you must invoke the <E> command if you want those
new definitions updated in the EEPROM.  You can also clear everything stored in the EEPROM by invoking the <e> command.

To change the state of outputs that have been defined use:

  <Z ID STATE>:                sets output ID to either ACTIVE or INACTIVE state
                               returns: <Y ID STATE>, or <X> if turnout ID does not exist

where

  ID: the numeric ID (0-32767) of the turnout to control
  STATE: the state of the output (0=INACTIVE / 1=ACTIVE)

When controlled as such, the Arduino updates and stores the direction of each output in EEPROM so
that it is retained even without power.  A list of the current states of each output in the form <Y ID STATE> is generated
by this sketch whenever the <s> status command is invoked.  This provides an efficient way of initializing
the state of any outputs being monitored or controlled by a separate interface or GUI program.

**********************************************************************/

#include "Outputs.h"
#include "SerialCommand.h"


void Output::activate(int s){
  data.oStatus=(s>0);                                               // if s>0, set status to active, else inactive
  digitalWrite(data.pin,data.oStatus ^ bitRead(data.iFlag,0));      // set state of output pin to HIGH or LOW depending on whether bit zero of iFlag is set to 0 (ACTIVE=HIGH) or 1 (ACTIVE=LOW)
  // if(num>0)
  //   EEPROM.put(num,data.oStatus);
  Serial.print("<Y");
  Serial.print(data.id);
  if(data.oStatus==0)
    Serial.print(" 0>");
  else
    Serial.print(" 1>"); 
}


Output* Output::get(int n){
  Output *tt;
  for(tt=firstOutput;tt!=NULL && tt->data.id!=n;tt=tt->nextOutput);
  return(tt); 
}


void Output::remove(int n){
  Output *tt,*pp;
  
  for(tt=firstOutput;tt!=NULL && tt->data.id!=n;pp=tt,tt=tt->nextOutput);

  if(tt==NULL){
    Serial.print("<X>");
    return;
  }
  
  if(tt==firstOutput)
    firstOutput=tt->nextOutput;
  else
    pp->nextOutput=tt->nextOutput;

  free(tt);

  Serial.print("<O>");
}


void Output::show(int n){
  Output *tt;

  if(firstOutput==NULL){
    Serial.print("<X>");
    return;
  }
    
  for(tt=firstOutput;tt!=NULL;tt=tt->nextOutput){
    Serial.print("<Y");
    Serial.print(tt->data.id);
    if(n==1){
      Serial.print(" ");
      Serial.print(tt->data.pin);
      Serial.print(" ");
      Serial.print(tt->data.iFlag);
    }
    if(tt->data.oStatus==0)
       Serial.print(" 0>");
     else
       Serial.print(" 1>"); 
  }
}


void Output::parse(char *c){
  int n,s,m;
  Output *t;
  
  switch(sscanf(c,"%d %d %d",&n,&s,&m)){
    
    case 2:                     // argument is string with id number of output followed by zero (LOW) or one (HIGH)
      t=get(n);
      if(t!=NULL)
        t->activate(s);
      else
        Serial.print("<X>");
      break;

    case 3:                     // argument is string with id number of output followed by a pin number and invert flag
      create(n,s,m,1);
    break;

    case 1:                     // argument is a string with id number only
      remove(n);
    break;
    
    case -1:                    // no arguments
      show(1);                  // verbose show
    break;
  }
}


Output *Output::create(int id, int pin, int iFlag, int v){
  Output *tt;
  
  if(firstOutput==NULL){
    firstOutput=(Output *)calloc(1,sizeof(Output));
    tt=firstOutput;
  } else if((tt=get(id))==NULL){
    tt=firstOutput;
    while(tt->nextOutput!=NULL)
      tt=tt->nextOutput;
    tt->nextOutput=(Output *)calloc(1,sizeof(Output));
    tt=tt->nextOutput;
  }

  if(tt==NULL){       // problem allocating memory
    if(v==1)
      Serial.print("<X>");
    return(tt);
  }
  
  tt->data.id=id;
  tt->data.pin=pin;
  tt->data.iFlag=iFlag;
  tt->data.oStatus=0;
  
  if(v==1){
    tt->data.oStatus=bitRead(tt->data.iFlag,1)?bitRead(tt->data.iFlag,2):0;      // sets status to 0 (INACTIVE) is bit 1 of iFlag=0, otherwise set to value of bit 2 of iFlag  
    digitalWrite(tt->data.pin,tt->data.oStatus ^ bitRead(tt->data.iFlag,0));
    pinMode(tt->data.pin,OUTPUT);
    Serial.print("<O>");
  }
  
  return(tt);
  
}


Output *Output::firstOutput=NULL;

#line 1 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
#include <Arduino.h>
#include <Motor.h>
#include <RHReliableDatagram.h>
#include <RH_RF69.h>
#include "PacketRegister.h"
#include "SerialCommand.h"

// Radio identifiers
#define CLIENT_ADDRESS 101
#define SERVER_ADDRESS 202

// Radio initialization
#define RF69_FREQ 868.0
#define RFM69_CS 8
#define RFM69_INT 7
RH_RF69 driver(RFM69_CS, RFM69_INT);
RHReliableDatagram manager(driver, SERVER_ADDRESS);
uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];

// create list of registers for MAX_MAIN_REGISTER Main Track Packets
volatile RegisterList mainRegs(1);
uint8_t *command;

// DCC output pins
#define DCC_1 9
#define DCC_2 10

// Timer duration and interrupt values for 0/1 bits
#define DCC_ZERO_BIT_TOTAL_DURATION 199
#define DCC_ZERO_BIT_PULSE_DURATION 99
#define DCC_ONE_BIT_TOTAL_DURATION 115
#define DCC_ONE_BIT_PULSE_DURATION 57

void setup()
{
    SerialCommand::init(&mainRegs); // create structure to read and parse commands from serial line

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

    // Set DCC pins out output
    pinMode(DCC_1, OUTPUT);
    pinMode(DCC_2, OUTPUT);

    // 32u4 Timer 4, 10 bit
    // Enable Timer 4B output
    bitSet(DDRB, 6);

    // Set waveform mode to up/reset
    bitClear(TCCR4D, WGM40);
    bitClear(TCCR4D, WGM41);

    // Set prescale to /8
    bitClear(TCCR4B, CS40);
    bitClear(TCCR4B, CS41);
    bitSet(TCCR4B, CS42);
    bitClear(TCCR4B, CS43);

    // Set comparator to toggle/inverted
    bitSet(TCCR4A, PWM4B);
    bitSet(TCCR4A, COM4B0);
    bitClear(TCCR4A, COM4B1);

    // Set one bits as default write
    OCR4C = DCC_ONE_BIT_TOTAL_DURATION;
    OCR4B = DCC_ONE_BIT_PULSE_DURATION;

    // load idle packet into register 1
    mainRegs.loadPacket(1, RegisterList::idlePacket, 2, 0);

    // Enable interrupt vector
    bitSet(TIMSK4, OCIE4B);

    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
}

void loop()
{
    command = 0;
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
            command = buf;
            SerialCommand::process(command); // check for, and process, and new serial commands
        }
    }
}

#define DCC_SIGNAL                                                                                                                                                                          \
    if (mainRegs.currentBit == mainRegs.currentReg->activePacket->nBits)                                                                                                                    \
    {                            /* IF no more bits in this DCC Packet */                                                                                                                   \
        mainRegs.currentBit = 0; /* reset current bit pointer and determine which Register and Packet to process next--- */                                                                 \
                                                                                                                                                                                            \
        if (mainRegs.nRepeat > 0 && mainRegs.currentReg == mainRegs.reg)                                                                                                                    \
        {                       /* IF current Register is first Register AND should be repeated */                                                                                          \
            mainRegs.nRepeat--; /* decrement repeat count; result is this same Packet will be repeated */                                                                                   \
        }                                                                                                                                                                                   \
        else if (mainRegs.nextReg != NULL)                                                                                                                                                  \
        {                                                            /* ELSE IF another Register has been updated */                                                                        \
            mainRegs.currentReg = mainRegs.nextReg;                  /* update currentReg to nextReg */                                                                                     \
            mainRegs.nextReg = NULL;                                 /* reset nextReg to NULL */                                                                                            \
            mainRegs.tempPacket = mainRegs.currentReg->activePacket; /* flip active and update Packets */                                                                                   \
                                                                                                                                                                                            \
            mainRegs.currentReg->activePacket = mainRegs.currentReg->updatePacket;                                                                                                          \
            mainRegs.currentReg->updatePacket = mainRegs.tempPacket;                                                                                                                        \
        }                                                                                                                                                                                   \
        else                                                                                                                                                                                \
        {                                                     /* ELSE simply move to next Register */                                                                                       \
            if (mainRegs.currentReg == mainRegs.maxLoadedReg) /* BUT IF this is last Register loaded */                                                                                     \
                mainRegs.currentReg = mainRegs.reg;           /* first reset currentReg to base Register, THEN */                                                                           \
            mainRegs.currentReg++;                            /* increment current Register (note this logic causes Register[0] to be skipped when simply cycling through all Registers) */ \
        }                                                                                                                                                                                   \
    } /* END-IF: currentReg, activePacket, and currentBit should now be properly set to point to next DCC bit */                                                                            \
                                                                                                                                                                                            \
    if (mainRegs.currentReg->activePacket->buf[mainRegs.currentBit / 8] & mainRegs.bitMask[mainRegs.currentBit % 8])                                                                        \
    {                                       /* IF bit is a ONE */                                                                                                                           \
        OCR4C = DCC_ONE_BIT_TOTAL_DURATION; /* set OCRA for timer N to full cycle duration of DCC ONE bit */                                                                                \
        OCR4B = DCC_ONE_BIT_PULSE_DURATION; /* set OCRB for timer N to half cycle duration of DCC ONE but */                                                                                \
    }                                                                                                                                                                                       \
    else                                                                                                                                                                                    \
    {                                        /* ELSE it is a ZERO */                                                                                                                        \
        OCR4C = DCC_ZERO_BIT_TOTAL_DURATION; /* set OCRA for timer N to full cycle duration of DCC ZERO bit */                                                                              \
        OCR4B = DCC_ZERO_BIT_PULSE_DURATION; /* set OCRB for timer N to half cycle duration of DCC ZERO bit */                                                                              \
    }                                        /* END-ELSE */                                                                                                                                 \
                                                                                                                                                                                            \
    mainRegs.currentBit++; /* point to next bit in current Packet */

///////////////////////////////////////////////////////////////////////////////

// NOW USE THE ABOVE MACRO TO CREATE THE CODE FOR EACH INTERRUPT

ISR(TIMER4_COMPB_vect)
{ // set interrupt service for OCR1B of TIMER-1 which flips direction bit of Motor Shield Channel A controlling Main Track
    DCC_SIGNAL
}

