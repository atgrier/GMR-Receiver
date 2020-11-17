# 1 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\Outputs.cpp"
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

# 75 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\Outputs.cpp" 2
# 76 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\Outputs.cpp" 2


void Output::activate(int s){
  data.oStatus=(s>0); // if s>0, set status to active, else inactive
  digitalWrite(data.pin,data.oStatus ^ (((data.iFlag) >> (0)) & 0x01)); // set state of output pin to HIGH or LOW depending on whether bit zero of iFlag is set to 0 (ACTIVE=HIGH) or 1 (ACTIVE=LOW)
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
  for(tt=firstOutput;tt!=
# 94 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\Outputs.cpp" 3 4
                        __null 
# 94 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\Outputs.cpp"
                             && tt->data.id!=n;tt=tt->nextOutput);
  return(tt);
}


void Output::remove(int n){
  Output *tt,*pp;

  for(tt=firstOutput;tt!=
# 102 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\Outputs.cpp" 3 4
                        __null 
# 102 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\Outputs.cpp"
                             && tt->data.id!=n;pp=tt,tt=tt->nextOutput);

  if(tt==
# 104 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\Outputs.cpp" 3 4
        __null
# 104 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\Outputs.cpp"
            ){
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

  if(firstOutput==
# 123 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\Outputs.cpp" 3 4
                 __null
# 123 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\Outputs.cpp"
                     ){
    Serial.print("<X>");
    return;
  }

  for(tt=firstOutput;tt!=
# 128 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\Outputs.cpp" 3 4
                        __null
# 128 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\Outputs.cpp"
                            ;tt=tt->nextOutput){
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

    case 2: // argument is string with id number of output followed by zero (LOW) or one (HIGH)
      t=get(n);
      if(t!=
# 153 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\Outputs.cpp" 3 4
           __null
# 153 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\Outputs.cpp"
               )
        t->activate(s);
      else
        Serial.print("<X>");
      break;

    case 3: // argument is string with id number of output followed by a pin number and invert flag
      create(n,s,m,1);
    break;

    case 1: // argument is a string with id number only
      remove(n);
    break;

    case -1: // no arguments
      show(1); // verbose show
    break;
  }
}


Output *Output::create(int id, int pin, int iFlag, int v){
  Output *tt;

  if(firstOutput==
# 177 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\Outputs.cpp" 3 4
                 __null
# 177 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\Outputs.cpp"
                     ){
    firstOutput=(Output *)calloc(1,sizeof(Output));
    tt=firstOutput;
  } else if((tt=get(id))==
# 180 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\Outputs.cpp" 3 4
                         __null
# 180 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\Outputs.cpp"
                             ){
    tt=firstOutput;
    while(tt->nextOutput!=
# 182 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\Outputs.cpp" 3 4
                         __null
# 182 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\Outputs.cpp"
                             )
      tt=tt->nextOutput;
    tt->nextOutput=(Output *)calloc(1,sizeof(Output));
    tt=tt->nextOutput;
  }

  if(tt==
# 188 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\Outputs.cpp" 3 4
        __null
# 188 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\Outputs.cpp"
            ){ // problem allocating memory
    if(v==1)
      Serial.print("<X>");
    return(tt);
  }

  tt->data.id=id;
  tt->data.pin=pin;
  tt->data.iFlag=iFlag;
  tt->data.oStatus=0;

  if(v==1){
    tt->data.oStatus=(((tt->data.iFlag) >> (1)) & 0x01)?(((tt->data.iFlag) >> (2)) & 0x01):0; // sets status to 0 (INACTIVE) is bit 1 of iFlag=0, otherwise set to value of bit 2 of iFlag  
    digitalWrite(tt->data.pin,tt->data.oStatus ^ (((tt->data.iFlag) >> (0)) & 0x01));
    pinMode(tt->data.pin,0x1);
    Serial.print("<O>");
  }

  return(tt);

}


Output *Output::firstOutput=
# 211 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\Outputs.cpp" 3 4
                           __null
# 211 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\Outputs.cpp"
                               ;
# 1 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
# 2 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 2
# 3 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 2
# 4 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 2
# 5 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 2
# 6 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 2
# 7 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 2

// Radio identifiers



// Radio initialization



RH_RF69 driver(8, 7);
RHReliableDatagram manager(driver, 202);
uint8_t buf[(64 - 4)];

// create list of registers for MAX_MAIN_REGISTER Main Track Packets
volatile RegisterList mainRegs(1);
uint8_t *command;

// DCC output pins



// Timer duration and interrupt values for 0/1 bits





void setup()
{
    SerialCommand::init(&mainRegs); // create structure to read and parse commands from serial line

    Serial.begin(115200); // configure serial interface
    Serial.flush();

    // Initialize radio
    manager.init();
    driver.setFrequency(868.0);
    driver.setTxPower(20, true);
    uint8_t key[] = {0xa, 0xb, 0xa, 0xd, 0xc, 0xa, 0xf, 0xe,
                     0xd, 0xe, 0xa, 0xd, 0xb, 0xe, 0xe, 0xf};
    driver.setEncryptionKey(key);
    manager.setTimeout(50);

    // Set DCC pins out output
    pinMode(9, 0x1);
    pinMode(10, 0x1);

    // 32u4 Timer 4, 10 bit
    // Enable Timer 4B output
    ((
# 56 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 3
   (*(volatile uint8_t *)((0x04) + 0x20))
# 56 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
   ) |= (1UL << (6)));

    // Set waveform mode to up/reset
    ((
# 59 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 3
   (*(volatile uint8_t *)(0xC3))
# 59 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
   ) &= ~(1UL << (
# 59 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 3
   0
# 59 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
   )));
    ((
# 60 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 3
   (*(volatile uint8_t *)(0xC3))
# 60 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
   ) &= ~(1UL << (
# 60 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 3
   1
# 60 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
   )));

    // Set prescale to /8
    ((
# 63 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 3
   (*(volatile uint8_t *)(0xC1))
# 63 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
   ) &= ~(1UL << (
# 63 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 3
   0
# 63 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
   )));
    ((
# 64 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 3
   (*(volatile uint8_t *)(0xC1))
# 64 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
   ) &= ~(1UL << (
# 64 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 3
   1
# 64 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
   )));
    ((
# 65 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 3
   (*(volatile uint8_t *)(0xC1))
# 65 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
   ) |= (1UL << (
# 65 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 3
   2
# 65 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
   )));
    ((
# 66 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 3
   (*(volatile uint8_t *)(0xC1))
# 66 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
   ) &= ~(1UL << (
# 66 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 3
   3
# 66 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
   )));

    // Set comparator to toggle/inverted
    ((
# 69 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 3
   (*(volatile uint8_t *)(0xC0))
# 69 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
   ) |= (1UL << (
# 69 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 3
   0
# 69 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
   )));
    ((
# 70 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 3
   (*(volatile uint8_t *)(0xC0))
# 70 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
   ) |= (1UL << (
# 70 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 3
   4
# 70 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
   )));
    ((
# 71 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 3
   (*(volatile uint8_t *)(0xC0))
# 71 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
   ) &= ~(1UL << (
# 71 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 3
   5
# 71 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
   )));

    // Set one bits as default write
    
# 74 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 3
   (*(volatile uint8_t *)(0xD1)) 
# 74 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
         = 115;
    
# 75 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 3
   (*(volatile uint8_t *)(0xD0)) 
# 75 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
         = 57;

    // load idle packet into register 1
    mainRegs.loadPacket(1, RegisterList::idlePacket, 2, 0);

    // Enable interrupt vector
    ((
# 81 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 3
   (*(volatile uint8_t *)(0x72))
# 81 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
   ) |= (1UL << (
# 81 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 3
   5
# 81 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
   )));

    pinMode(13, 0x1);
    digitalWrite(13, 0x1);
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
# 147 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
///////////////////////////////////////////////////////////////////////////////

// NOW USE THE ABOVE MACRO TO CREATE THE CODE FOR EACH INTERRUPT


# 151 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 3
extern "C" void __vector_39 /* Timer/Counter4 Compare Match B */ (void) __attribute__ ((signal,used, externally_visible)) ; void __vector_39 /* Timer/Counter4 Compare Match B */ (void)

# 152 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
{ // set interrupt service for OCR1B of TIMER-1 which flips direction bit of Motor Shield Channel A controlling Main Track
    if (mainRegs.currentBit == mainRegs.currentReg->activePacket->nBits) { /* IF no more bits in this DCC Packet */ mainRegs.currentBit = 0; /* reset current bit pointer and determine which Register and Packet to process next--- */ if (mainRegs.nRepeat > 0 && mainRegs.currentReg == mainRegs.reg) { /* IF current Register is first Register AND should be repeated */ mainRegs.nRepeat--; /* decrement repeat count; result is this same Packet will be repeated */ } else if (mainRegs.nextReg != 
# 153 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 3 4
   __null
# 153 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
   ) { /* ELSE IF another Register has been updated */ mainRegs.currentReg = mainRegs.nextReg; /* update currentReg to nextReg */ mainRegs.nextReg = 
# 153 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 3 4
   __null
# 153 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
   ; /* reset nextReg to NULL */ mainRegs.tempPacket = mainRegs.currentReg->activePacket; /* flip active and update Packets */ mainRegs.currentReg->activePacket = mainRegs.currentReg->updatePacket; mainRegs.currentReg->updatePacket = mainRegs.tempPacket; } else { /* ELSE simply move to next Register */ if (mainRegs.currentReg == mainRegs.maxLoadedReg) /* BUT IF this is last Register loaded */ mainRegs.currentReg = mainRegs.reg; /* first reset currentReg to base Register, THEN */ mainRegs.currentReg++; /* increment current Register (note this logic causes Register[0] to be skipped when simply cycling through all Registers) */ } } /* END-IF: currentReg, activePacket, and currentBit should now be properly set to point to next DCC bit */ if (mainRegs.currentReg->activePacket->buf[mainRegs.currentBit / 8] & mainRegs.bitMask[mainRegs.currentBit % 8]) { /* IF bit is a ONE */ 
# 153 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 3
   (*(volatile uint8_t *)(0xD1)) 
# 153 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
   = 115; /* set OCRA for timer N to full cycle duration of DCC ONE bit */ 
# 153 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 3
   (*(volatile uint8_t *)(0xD0)) 
# 153 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
   = 57; /* set OCRB for timer N to half cycle duration of DCC ONE but */ } else { /* ELSE it is a ZERO */ 
# 153 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 3
   (*(volatile uint8_t *)(0xD1)) 
# 153 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
   = 199; /* set OCRA for timer N to full cycle duration of DCC ZERO bit */ 
# 153 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino" 3
   (*(volatile uint8_t *)(0xD0)) 
# 153 "c:\\Users\\Alan\\Google Drive\\Documents\\Arduino\\wireless_loco\\great_northern_reciever\\great_northern_receiver.ino"
   = 99; /* set OCRB for timer N to half cycle duration of DCC ZERO bit */ } /* END-ELSE */ mainRegs.currentBit++; /* point to next bit in current Packet */
}
