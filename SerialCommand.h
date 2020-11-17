/**********************************************************************

SerialCommand.h
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

#ifndef SerialCommand_h
#define SerialCommand_h

#include "PacketRegister.h"

#define MAX_COMMAND_LENGTH 30

struct SerialCommand
{
  static char commandString[MAX_COMMAND_LENGTH + 1];
  static volatile RegisterList *mRegs;
  static void init(volatile RegisterList *);
  static void parse(char *);
  static void process(uint8_t *);
};

#endif
