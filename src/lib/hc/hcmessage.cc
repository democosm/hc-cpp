// HC message
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

#include "hcmessage.hh"
#include "error.hh"
#include "thread.hh"
#include <cassert>
#include <iomanip>
#include <iostream>

using namespace std;

HCMessage::HCMessage()
{
  //Allocate memory for message buffer
  _buffer = new uint8_t[OVERHEAD + PAYLOAD_MAX];

  //Remember payload pointer (offset within message buffer)
  _payload = _buffer + OVERHEAD;

  //Initialize read index, payload length and transaction number
  _readindex = 0;
  _payloadlength = 0;
  _transaction = 0;
}

HCMessage::~HCMessage()
{
  //Deallocate memory for message buffer
  delete[] _buffer;
}

void HCMessage::Reset(uint8_t transaction)
{
  //Initialize read index, payload length and transaction number
  _readindex = 0;
  _payloadlength = 0;
  _transaction = transaction;
}

uint8_t HCMessage::GetTransaction(void)
{
  return _transaction;
}

int HCMessage::Send(Device *dev)
{
  uint32_t i;

  //Assert valid arguments
  assert(dev != 0);

  //Zero index
  i=0;

  //Serialize transaction number
  _buffer[i++] = _transaction;

  //Payload is already serialized in buffer so skip
  i += _payloadlength;

  //Write serialized message to device and check for error
  if(dev->Write(_buffer, i) != i)
    return ERR_UNSPEC;

  //Success
  return ERR_NONE;
}

int HCMessage::Recv(Device *dev)
{
  uint32_t rcnt;
  uint32_t i;

  //Assert valid arguments
  assert(dev != 0);

  //Read serialized message from device and check for error
  if((rcnt = dev->Read(_buffer, OVERHEAD + PAYLOAD_MAX)) == 0)
  {
    //Sleep a while to prevent starving other threads
    ThreadSleep(1000000);
    return ERR_UNSPEC;
  }

  //Check for overflow
  if((rcnt < OVERHEAD) || (rcnt > (OVERHEAD + PAYLOAD_MAX)))
    return ERR_UNSPEC;

  //Zero index
  i=0;

  //Deserialize transaction number
  _transaction = _buffer[i++];

  //Reset read index
  _readindex = 0;

  //Set payload length
  _payloadlength = rcnt - OVERHEAD;

  //Success
  return ERR_NONE;
}

bool HCMessage::Read(HCCell *cell)
{
  uint32_t len;

  //Deserialize cell from payload and check for error
  if((len = cell->Deserialize(_payload + _readindex, _payloadlength - _readindex)) == 0)
    return false;

  //Update read index
  _readindex += len;

  return true;
}

bool HCMessage::Write(HCCell *cell)
{
  uint32_t len;

  //Serialize cell into payload and check for error
  if((len = cell->Serialize(_payload + _payloadlength, PAYLOAD_MAX - _payloadlength)) == 0)
    return false;

  //Update payload length
  _payloadlength += len;

  return true;
}

void HCMessage::Print(const string &extra)
{
  uint32_t i;

  //Print common info
  cout << extra << "Msg: Xact=" << (uint16_t)_transaction;

  //Print payload
  cout << ", Payload=";
  for(i=0; i<_payloadlength; i++)
    cout << hex << setw(2) << setfill('0') << (uint16_t)_payload[i] << dec << " ";
  cout << "\n";
}
