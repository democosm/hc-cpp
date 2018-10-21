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

#include "error.hh"
#include "ftoi.hh"
#include "piserver.hh"
#include "thread.hh"
#include <stdio.h>
#include <stdlib.h>

PIServer::PIServer()
{
  //Create relay GPIO drivers
  _relay[0] = new PIGPIO(26, true, true);
  _relay[1] = new PIGPIO(20, true, true);
  _relay[2] = new PIGPIO(21, true, true);

  //Initialize CPU utilization status
  _cpuutilization = 0.0;

  //Create and start control thread
  _ctlthread = new Thread<PIServer>(this, &PIServer::CtlThread);
  _ctlthread->Start();
}

PIServer::~PIServer()
{
  //Delete control thread
  delete _ctlthread;

  //Delete relay GPIO drivers
  delete _relay[0];
  delete _relay[1];
  delete _relay[2];
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

int PIServer::GetCPUUtilization(uint8_t &val)
{
  val = _cpuutilization;
  return ERR_NONE;
}

int PIServer::GetRelayOn(uint32_t eid, bool &val)
{
  bool gpioval;
  int retval;

  //Check for invalid id
  if(eid >= 3)
    return ERR_EID;

  //Get GPIO value and invert since relays are active low driven
  retval = _relay[eid]->GetValue(gpioval);
  val = gpioval ? false : true;

  return retval;
}

int PIServer::SetRelayOn(uint32_t eid, bool val)
{
  //Check for invalid id
  if(eid >= 3)
    return ERR_EID;

  //Set GPIO value to inverse since relays are active low driven
  return _relay[eid]->SetValue(val ? false : true);
}

int PIServer::PulseRelayHigh(uint32_t eid)
{
  //Turn relay on, sleep and set turn relay off
  SetRelayOn(eid, true);
  ThreadSleep(500000);
  SetRelayOn(eid, false);

  return ERR_NONE;
}

void PIServer::CtlThread(void)
{
  uint64_t user;
  uint64_t nice;
  uint64_t sys;
  uint64_t idle;
  uint64_t totaluse;
  uint64_t total;
  uint64_t oldtotaluse;
  uint64_t oldtotal;
  FILE *fp;

  //Initialize old counts
  oldtotaluse = 0;
  oldtotal = 0;

  //Go forever
  while(true)
  {
    //Sleep one second
    ThreadSleep(1000000);

    //Open processor stats file and check for success
    if((fp = fopen("/proc/stat", "r")) != NULL)
    {
      //Read in stats and check for success
      if(fscanf(fp, "%*s %" SCNu64 " %" SCNu64 " %" SCNu64 " %" SCNu64, &user, &nice, &sys, &idle) == 4)
      {
        //Calculate totals
        totaluse = user + nice + sys;
        total = totaluse + idle;

        //Don't do this on the first iteration
        if(oldtotal != 0)
          FToU((double)(totaluse - oldtotaluse)/(double)(total - oldtotal)*100.0, _cpuutilization);

        //These are now the old stats
        oldtotaluse = totaluse;
        oldtotal = total;
      }

      //Close processor stats file
      fclose(fp);
    }
  }
}
