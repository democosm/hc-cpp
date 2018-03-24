// Class for controlling a raspberry pi 3
//
// Copyright 2018 Democosm
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include "piserver.hh"
#include "error.hh"
#include "thread.hh"
#include <stdio.h>
#include <stdlib.h>

uint32_t Relaygpiomap[8] = { 4, 17, 18, 27, 22, 23, 24, 25 };

PIServer::PIServer()
{
  uint32_t i;
  char command[200];

  //Create GPIO control files for relay board
  for(i=0; i<8; i++)
  {
    sprintf(command, "echo \"%" PRIu32 "\" > /sys/class/gpio/export", Relaygpiomap[i]);
    system(command);
  }

  //Takes a while for files to show up
  ThreadSleep(100000);

  //Set direction of all relay GPIOs to output
  for(i=0; i<8; i++)
  {
    sprintf(command, "echo \"out\" > /sys/class/gpio/gpio%" PRIu32 "/direction", Relaygpiomap[i]);
    system(command);
  }

  //Set all relay GPIOs to off (relays are active low controlled)
  for(i=0; i<8; i++)
  {
    sprintf(command, "echo \"1\" > /sys/class/gpio/gpio%" PRIu32 "/value", Relaygpiomap[i]);
    system(command);
  }
}

PIServer::~PIServer()
{
  uint32_t i;
  char command[200];

  //Set direction of all relay GPIOs to input
  for(i=0; i<8; i++)
  {
    sprintf(command, "echo \"in\" > /sys/class/gpio/gpio%" PRIu32 "/direction", Relaygpiomap[i]);
    system(command);
  }

  //Destroy GPIO control files for relay board
  for(i=0; i<8; i++)
  {
    sprintf(command, "echo \"%" PRIu32 "\" > /sys/class/gpio/unexport", Relaygpiomap[i]);
    system(command);
  }

  //Assum it takes a while for files to go away
  ThreadSleep(100000);
}

int PIServer::GetTemperature(float &val)
{
  FILE *file;
  uint32_t valint;

  //Open temperature file and check for error
  if((file = fopen("/sys/class/thermal/thermal_zone0/temp", "r")) == NULL)
    return ERR_ACCESS;

  //Read temperature from file
  fscanf(file, "%" SCNu32, &valint);

  //Temperature from file is in milli-degC, so convert to degC
  val = (float)valint / 1000.0;

  //Close file
  fclose(file);

  return ERR_NONE;
}

int PIServer::GetRelayOn(uint32_t eid, bool &val)
{
  char filename[200];
  FILE *file;
  uint32_t valint;

  //Check for invalid EID
  if(eid >= 8)
    return ERR_EID;

  //Open file for manipulating relay GPIO value
  sprintf(filename, "/sys/class/gpio/gpio%" PRIu32 "/value", Relaygpiomap[eid]);

  if((file = fopen(filename, "r")) == NULL)
    printf("Error opening GPIO %" PRIu32 " file\n", Relaygpiomap[eid]);

  //Read relay value from file
  fscanf(file, "%" SCNu32, &valint);

  //Convert to bool
  val = valint == 1 ? false : true;

  //Close file
  fclose(file);

  return ERR_NONE;
}

int PIServer::SetRelayOn(uint32_t eid, bool val)
{
  char filename[200];
  FILE *file;

  //Check for invalid EID
  if(eid >= 8)
    return ERR_EID;

  //Open file for manipulating relay GPIO value
  sprintf(filename, "/sys/class/gpio/gpio%" PRIu32 "/value", Relaygpiomap[eid]);

  if((file = fopen(filename, "w")) == NULL)
    printf("Error opening GPIO %" PRIu32 " file\n", Relaygpiomap[eid]);

  //Write relay value to file
  fprintf(file, "%" PRIu32, val ? 0 : 1);

  //Close file
  fclose(file);

  return ERR_NONE;
}

int PIServer::PulseRelayHigh(uint32_t eid)
{
  //Turn relay on, sleep and set turn relay off
  SetRelayOn(eid, true);
  ThreadSleep(250000);
  SetRelayOn(eid, false);

  return ERR_NONE;
}
