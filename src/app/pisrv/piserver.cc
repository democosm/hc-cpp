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

PIServer::PIServer()
{
  //Create relay GPIO drivers
  m_relay[0] = new PIGPIO(4, true, true);
  m_relay[1] = new PIGPIO(17, true, true);
  m_relay[2] = new PIGPIO(18, true, true);
  m_relay[3] = new PIGPIO(27, true, true);
  m_relay[4] = new PIGPIO(22, true, true);
  m_relay[5] = new PIGPIO(23, true, true);
  m_relay[6] = new PIGPIO(24, true, true);
  m_relay[7] = new PIGPIO(25, true, true);
}

PIServer::~PIServer()
{
  //Delete relay GPIO drivers
  delete m_relay[0];
  delete m_relay[1];
  delete m_relay[2];
  delete m_relay[3];
  delete m_relay[4];
  delete m_relay[5];
  delete m_relay[6];
  delete m_relay[7];
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
  bool gpioval;
  int retval;

  //Check for invalid id
  if(eid >= 8)
    return ERR_EID;

  //Get GPIO value and invert since relays are active low driven
  retval = m_relay[eid]->GetValue(gpioval);
  val = gpioval ? false : true;

  return retval;
}

int PIServer::SetRelayOn(uint32_t eid, bool val)
{
  //Check for invalid id
  if(eid >= 8)
    return ERR_EID;

  //Set GPIO value to inverse since relays are active low driven
  return m_relay[eid]->SetValue(val ? false : true);
}

int PIServer::PulseRelayHigh(uint32_t eid)
{
  //Turn relay on, sleep and set turn relay off
  SetRelayOn(eid, true);
  ThreadSleep(50000);
  SetRelayOn(eid, false);

  return ERR_NONE;
}
