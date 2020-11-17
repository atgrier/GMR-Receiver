/**********************************************************************

SerialCommand.cpp
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

// DCC++ BASE STATION COMMUNICATES VIA THE SERIAL PORT USING SINGLE-CHARACTER TEXT COMMANDS
// WITH OPTIONAL PARAMTERS, AND BRACKETED BY < AND > SYMBOLS.  SPACES BETWEEN PARAMETERS
// ARE REQUIRED.  SPACES ANYWHERE ELSE ARE IGNORED.  A SPACE BETWEEN THE SINGLE-CHARACTER
// COMMAND AND THE FIRST PARAMETER IS ALSO NOT REQUIRED.

#include "SerialCommand.h"
#include "Outputs.h"

extern int __heap_start, *__brkval;

char SerialCommand::commandString[MAX_COMMAND_LENGTH + 1];
volatile RegisterList *SerialCommand::mRegs;

void SerialCommand::init(volatile RegisterList *_mRegs)
{
  mRegs = _mRegs;
  sprintf(commandString, "");
}

void SerialCommand::process(uint8_t *command)
{
  char c;
  if (command != 0)
  {
    parse((char *)(command+1));
  }
  else
  {
    while (Serial.available() > 0)
    { // while there is data on the serial line
      c = Serial.read();
      if (c == '<') // start of new command
        sprintf(commandString, "");
      else if (c == '>') // end of new command
        parse(commandString);
      else if (strlen(commandString) < MAX_COMMAND_LENGTH) // if comandString still has space, append character just read from serial line
        sprintf(commandString, "%s%c", commandString, c);  // otherwise, character is ignored (but continue to look for '<' or '>')
    }
  }
}

void SerialCommand::parse(char *com)
{

  switch (com[0])
  {
    /***** SET ENGINE THROTTLES USING 128-STEP SPEED CONTROL ****/
  case 't': // <t REGISTER CAB SPEED DIRECTION>
            /*
 *    sets the throttle for a given register/cab combination 
 *    
 *    REGISTER: an internal register number, from 1 through MAX_MAIN_REGISTERS (inclusive), to store the DCC packet used to control this throttle setting
 *    CAB:  the short (1-127) or long (128-10293) address of the engine decoder
 *    SPEED: throttle speed from 0-126, or -1 for emergency stop (resets SPEED to 0)
 *    DIRECTION: 1=forward, 0=reverse.  Setting direction when speed=0 or speed=-1 only effects directionality of cab lighting for a stopped train
 *    
 *    returns: <T REGISTER SPEED DIRECTION>
 *    
 */
    mRegs->setThrottle(com + 1);
    break;


    /***** OPERATE ENGINE DECODER FUNCTIONS F0-F28 ****/
  case 'f': // <f CAB BYTE1 [BYTE2]>
            /*
 *    turns on and off engine decoder functions F0-F28 (F0 is sometimes called FL)  
 *    NOTE: setting requests transmitted directly to mobile engine decoder --- current state of engine functions is not stored by this program
 *    
 *    CAB:  the short (1-127) or long (128-10293) address of the engine decoder
 *    
 *    To set functions F0-F4 on (=1) or off (=0):
 *      
 *    BYTE1:  128 + F1*1 + F2*2 + F3*4 + F4*8 + F0*16
 *    BYTE2:  omitted
 *   
 *    To set functions F5-F8 on (=1) or off (=0):
 *   
 *    BYTE1:  176 + F5*1 + F6*2 + F7*4 + F8*8
 *    BYTE2:  omitted
 *   
 *    To set functions F9-F12 on (=1) or off (=0):
 *   
 *    BYTE1:  160 + F9*1 +F10*2 + F11*4 + F12*8
 *    BYTE2:  omitted
 *   
 *    To set functions F13-F20 on (=1) or off (=0):
 *   
 *    BYTE1: 222 
 *    BYTE2: F13*1 + F14*2 + F15*4 + F16*8 + F17*16 + F18*32 + F19*64 + F20*128
 *   
 *    To set functions F21-F28 on (=1) of off (=0):
 *   
 *    BYTE1: 223
 *    BYTE2: F21*1 + F22*2 + F23*4 + F24*8 + F25*16 + F26*32 + F27*64 + F28*128
 *   
 *    returns: NONE
 * 
 */
    mRegs->setFunction(com + 1);
    break;


    /***** CREATE/EDIT/REMOVE/SHOW & OPERATE AN OUTPUT PIN  ****/
  case 'Z': // <Z ID ACTIVATE>
            /*
 *   <Z ID ACTIVATE>:          sets output ID to either the "active" or "inactive" state
 *   
 *   ID: the numeric ID (0-32767) of the output to control
 *   ACTIVATE: 0 (active) or 1 (inactive)
 *   
 *   returns: <Y ID ACTIVATE> or <X> if output ID does not exist
 *   
 *   *** SEE OUTPUTS.CPP FOR COMPLETE INFO ON THE DIFFERENT VARIATIONS OF THE "O" COMMAND
 *   USED TO CREATE/EDIT/REMOVE/SHOW TURNOUT DEFINITIONS
 */
    Output::parse(com + 1);
    break;


/***** WRITE CONFIGURATION VARIABLE BYTE TO ENGINE DECODER ON MAIN OPERATIONS TRACK  ****/    
    case 'w':      // <w CAB CV VALUE>
/*
 *    writes, without any verification, a Configuration Variable to the decoder of an engine on the main operations track
 *    
 *    CAB:  the short (1-127) or long (128-10293) address of the engine decoder 
 *    CV: the number of the Configuration Variable memory location in the decoder to write to (1-1024)
 *    VALUE: the value to be written to the Configuration Variable memory location (0-255)
 *    
 *    returns: NONE
*/    
      mRegs->writeCVByteMain(com+1);
      break;      


/***** WRITE CONFIGURATION VARIABLE BIT TO ENGINE DECODER ON MAIN OPERATIONS TRACK  ****/    
    case 'b':      // <b CAB CV BIT VALUE>
/*
 *    writes, without any verification, a single bit within a Configuration Variable to the decoder of an engine on the main operations track
 *    
 *    CAB:  the short (1-127) or long (128-10293) address of the engine decoder 
 *    CV: the number of the Configuration Variable memory location in the decoder to write to (1-1024)
 *    BIT: the bit number of the Configurarion Variable regsiter to write (0-7)
 *    VALUE: the value of the bit to be written (0-1)
 *    
 *    returns: NONE
*/        
      mRegs->writeCVBitMain(com+1);
      break;


    /***** PRINT CARRIAGE RETURN IN SERIAL MONITOR WINDOW  ****/
  case ' ': // < >
            /*
 *    simply prints a carriage return - useful when interacting with Ardiuno through serial monitor window
 */
    Serial.println("");
    break;


    /***** WRITE A DCC PACKET TO ONE OF THE REGSITERS DRIVING THE MAIN OPERATIONS TRACK  ****/
  case 'M': // <M REGISTER BYTE1 BYTE2 [BYTE3] [BYTE4] [BYTE5]>
            /*
 *   writes a DCC packet of two, three, four, or five hexidecimal bytes to a register driving the main operations track
 *   FOR DEBUGGING AND TESTING PURPOSES ONLY.  DO NOT USE UNLESS YOU KNOW HOW TO CONSTRUCT NMRA DCC PACKETS - YOU CAN INADVERTENTLY RE-PROGRAM YOUR ENGINE DECODER
 *   
 *    REGISTER: an internal register number, from 0 through MAX_MAIN_REGISTERS (inclusive), to write (if REGISTER=0) or write and store (if REGISTER>0) the packet 
 *    BYTE1:  first hexidecimal byte in the packet
 *    BYTE2:  second hexidecimal byte in the packet
 *    BYTE3:  optional third hexidecimal byte in the packet
 *    BYTE4:  optional fourth hexidecimal byte in the packet
 *    BYTE5:  optional fifth hexidecimal byte in the packet
 */
    mRegs->writeTextPacket(com + 1);
    break;
  }
};
