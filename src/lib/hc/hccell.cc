// HC cell
//
// Copyright 2019 Democosm
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

#include "hccell.hh"
#include "error.hh"
#include "thread.hh"
#include <cassert>
#include <iomanip>
#include <iostream>
#include <string.h>

using namespace std;

HCCell::HCCell()
{
  //Allocate memory for cell buffer
  _buffer = new uint8_t[OVERHEAD + PAYLOAD_MAX];

  //Remember payload pointer (offset within cell buffer)
  _payload = _buffer + OVERHEAD;

  //Initialize read index, op code and payload length
  _readindex = 0;
  _opcode = 0;
  _payloadlength = 0;
}

HCCell::~HCCell()
{
  //Deallocate memory for buffer
  delete[] _buffer;
}

void HCCell::Reset(uint8_t opcode)
{
  //Initialize read index, op code and payload length
  _readindex = 0;
  _opcode = opcode;
  _payloadlength = 0;
}

uint8_t HCCell::GetOpCode(void)
{
  return _opcode;
}

uint32_t HCCell::Serialize(uint8_t* serbuf, uint32_t maxlen)
{
  uint32_t i;

  //Assert valid arguments
  assert((serbuf != 0) && (maxlen >= OVERHEAD));

  //Check for overflow
  if(maxlen < (OVERHEAD + _payloadlength))
    return 0;

  //Zero index
  i=0;

  //Serialize op code
  _buffer[i++] = _opcode;

  //Serialize payload length
  _buffer[i++] = (uint8_t)(_payloadlength >> 8);
  _buffer[i++] = (uint8_t)_payloadlength;

  //Payload is already serialized in buffer so skip
  i += _payloadlength;

  //Copy cell buffer to serialization buffer
  memcpy(serbuf, _buffer, i);

  //Return number of bytes serialized
  return i;
}

uint32_t HCCell::Deserialize(uint8_t* serbuf, uint32_t maxlen)
{
  uint32_t i;

  //Assert valid arguments
  assert(serbuf != 0);

  //Check for overflow
  if((maxlen < OVERHEAD) || (maxlen > (OVERHEAD + PAYLOAD_MAX)))
    return 0;

  //Zero index
  i=0;

  //Deserialize op code
  _opcode = serbuf[i++];

  //Deserialize payload length
  _payloadlength = (uint16_t)serbuf[i++] << 8;
  _payloadlength |= (uint16_t)serbuf[i++];

  //Check for underflow
  if(maxlen < (_payloadlength + i))
    return 0;

  //Copy payload from serialized buffer to cell payload
  memcpy(_payload, serbuf + i, _payloadlength);

  //Reset read index
  _readindex = 0;

  //Return number of bytes deserialized
  return _payloadlength + i;
}

bool HCCell::Read(bool& val)
{
  uint8_t temp;

  //Deserialize as same size integer and check for error
  if(!Read(temp))
    return false;

  //Special cast from integer to boolean
  val = (temp == 0 ? false : true);
  return true;
}

bool HCCell::Write(bool val)
{
  uint8_t temp;

  //Special cast from boolean to same size integer
  temp = val ? 1 : 0;

  //Delegate to integer version of this method
  return Write(temp);
}

bool HCCell::Read(string& val)
{
  uint32_t i;
  char ch;

  //Clear string
  val.clear();

  //Check for buffer underflow
  if(_readindex >= _payloadlength)
    return false;

  //Copy one character at a time
  for(i=0; _readindex<_payloadlength; i++)
  {
    //Get character at current read index
    ch = (char)_payload[_readindex++];

    //Check for null terminator
    if(ch == '\0')
      return true;

    //Copy character to value
    val += ch;
  }

  //This is an error case
  return false;
}

bool HCCell::Write(const string& val)
{
  uint32_t len;
  uint32_t i;

  //Determine length of string and terminator
  len = val.length();

  //Check for buffer overflow
  if((_payloadlength + len + 1) >= PAYLOAD_MAX)
    return false;

  //Copy characters
  for(i=0; i<len; i++)
    _payload[_payloadlength++] = (uint8_t)val.at(i);

  //Null terminate
  _payload[_payloadlength++] = (uint8_t)'\0';
  return true;
}

bool HCCell::Read(int8_t& val)
{
  //Delegate to unsigned version of this method
  return Read((uint8_t&)val);
}

bool HCCell::Write(int8_t val)
{
  //Delegate to unsigned version of this method
  return Write((uint8_t)val);
}

bool HCCell::Read(int16_t& val)
{
  //Delegate to unsigned version of this method
  return Read((uint16_t&)val);
}

bool HCCell::Write(int16_t val)
{
  //Delegate to unsigned version of this method
  return Write((uint16_t)val);
}

bool HCCell::Read(int32_t& val)
{
  //Delegate to unsigned version of this method
  return Read((uint32_t&)val);
}

bool HCCell::Write(int32_t val)
{
  //Delegate to unsigned version of this method
  return Write((uint32_t)val);
}

bool HCCell::Read(int64_t& val)
{
  //Delegate to unsigned version of this method
  return Read((uint64_t&)val);
}

bool HCCell::Write(int64_t val)
{
  //Delegate to unsigned version of this method
  return Write((uint64_t)val);
}

bool HCCell::Read(uint8_t& val)
{
  //Check for buffer underflow
  if((_payloadlength - _readindex) < sizeof(uint8_t))
    return false;

  //Deserialize value from payload
  val = _payload[_readindex++];
  return true;
}

bool HCCell::Write(uint8_t val)
{
  //Check for buffer overflow
  if((PAYLOAD_MAX - _payloadlength) < sizeof(uint8_t))
    return false;

  //Serialize value into payload
  _payload[_payloadlength++] = val;
  return true;
}

bool HCCell::Read(uint16_t& val)
{
  //Check for buffer underflow
  if((_payloadlength - _readindex) < sizeof(uint16_t))
    return false;

  //Deserialize value from payload
  val = (uint16_t)_payload[_readindex++] << 8;
  val |= (uint16_t)_payload[_readindex++];
  return true;
}

bool HCCell::Write(uint16_t val)
{
  //Check for buffer overflow
  if((PAYLOAD_MAX - _payloadlength) < sizeof(uint16_t))
    return false;

  //Serialize value into payload
  _payload[_payloadlength++] = (uint8_t)(val >> 8);
  _payload[_payloadlength++] = (uint8_t)val;
  return true;
}

bool HCCell::Read(uint32_t& val)
{
  //Check for buffer underflow
  if((_payloadlength - _readindex) < sizeof(uint32_t))
    return false;

  //Deserialize value from payload
  val = (uint32_t)_payload[_readindex++] << 24;
  val |= (uint32_t)_payload[_readindex++] << 16;
  val |= (uint32_t)_payload[_readindex++] << 8;
  val |= (uint32_t)_payload[_readindex++];
  return true;
}

bool HCCell::Write(uint32_t val)
{
  //Check for buffer overflow
  if((PAYLOAD_MAX - _payloadlength) < sizeof(uint32_t))
    return false;

  //Serialize value into payload
  _payload[_payloadlength++] = (uint8_t)(val >> 24);
  _payload[_payloadlength++] = (uint8_t)(val >> 16);
  _payload[_payloadlength++] = (uint8_t)(val >> 8);
  _payload[_payloadlength++] = (uint8_t)val;
  return true;
}

bool HCCell::Read(uint64_t& val)
{
  //Check for buffer underflow
  if((_payloadlength - _readindex) < sizeof(uint64_t))
    return false;

  //Deserialize value from payload
  val = (uint64_t)_payload[_readindex++] << 56;
  val |= (uint64_t)_payload[_readindex++] << 48;
  val |= (uint64_t)_payload[_readindex++] << 40;
  val |= (uint64_t)_payload[_readindex++] << 32;
  val |= (uint64_t)_payload[_readindex++] << 24;
  val |= (uint64_t)_payload[_readindex++] << 16;
  val |= (uint64_t)_payload[_readindex++] << 8;
  val |= (uint64_t)_payload[_readindex++];
  return true;
}

bool HCCell::Write(uint64_t val)
{
  //Check for buffer overflow
  if((PAYLOAD_MAX - _payloadlength) < sizeof(uint64_t))
    return false;

  //Serialize value into payload
  _payload[_payloadlength++] = (uint8_t)(val >> 56);
  _payload[_payloadlength++] = (uint8_t)(val >> 48);
  _payload[_payloadlength++] = (uint8_t)(val >> 40);
  _payload[_payloadlength++] = (uint8_t)(val >> 32);
  _payload[_payloadlength++] = (uint8_t)(val >> 24);
  _payload[_payloadlength++] = (uint8_t)(val >> 16);
  _payload[_payloadlength++] = (uint8_t)(val >> 8);
  _payload[_payloadlength++] = (uint8_t)val;
  return true;
}

template <typename T> bool HCCell::Read(T* val, uint16_t maxlen, uint16_t& len)
{
  uint32_t i;
  uint32_t j;

  //Assert valid arguments
  assert((val != 0) && (maxlen != 0));

  //Read length and check for error
  if(!Read(len))
    return false;

  //Check for buffer underflow
  if((_readindex + len * sizeof(T)) > _payloadlength)
    return false;

  //Copy values from payload
  for(i = 0; i < len; i++)
  {
    //Check for max length reached
    if(i >= maxlen)
      return true;

    //Copy value from payload
    val[i] = (T)_payload[_readindex++];
    for(j = 1; j < sizeof(T); j++)
      val[i] = (val[i] << 8) | _payload[_readindex++];
  }

  return true;
}

template bool HCCell::Read<int8_t>(int8_t* val, uint16_t maxlen, uint16_t& len);
template bool HCCell::Read<int16_t>(int16_t* val, uint16_t maxlen, uint16_t& len);
template bool HCCell::Read<int32_t>(int32_t* val, uint16_t maxlen, uint16_t& len);
template bool HCCell::Read<int64_t>(int64_t* val, uint16_t maxlen, uint16_t& len);
template bool HCCell::Read<uint8_t>(uint8_t* val, uint16_t maxlen, uint16_t& len);
template bool HCCell::Read<uint16_t>(uint16_t* val, uint16_t maxlen, uint16_t& len);
template bool HCCell::Read<uint32_t>(uint32_t* val, uint16_t maxlen, uint16_t& len);
template bool HCCell::Read<uint64_t>(uint64_t* val, uint16_t maxlen, uint16_t& len);

template <typename T> bool HCCell::Write(const T* val, uint16_t len)
{
  uint32_t i;
  uint32_t j;

  //Assert valid arguments
  assert(val != 0);

  //Write length and check for error
  if(!Write(len))
    return false;

  //Check for buffer overflow
  if((_payloadlength + len * sizeof(T)) > PAYLOAD_MAX)
    return false;

  //Copy values to payload
  for(i = 0; i < len; i++)
  {
    //Copy value to payload
    for(j = 0; j < sizeof(T); j++)
      _payload[_payloadlength++] = (uint8_t)(val[i] >> (8 * (sizeof(T) - j - 1)));
  }

  return true;
}

template bool HCCell::Write<int8_t>(const int8_t* val, uint16_t len);
template bool HCCell::Write<int16_t>(const int16_t* val, uint16_t len);
template bool HCCell::Write<int32_t>(const int32_t* val, uint16_t len);
template bool HCCell::Write<int64_t>(const int64_t* val, uint16_t len);
template bool HCCell::Write<uint8_t>(const uint8_t* val, uint16_t len);
template bool HCCell::Write<uint16_t>(const uint16_t* val, uint16_t len);
template bool HCCell::Write<uint32_t>(const uint32_t* val, uint16_t len);
template bool HCCell::Write<uint64_t>(const uint64_t* val, uint16_t len);

bool HCCell::Read(float& val)
{
  uint32_t temp;

  //Deserialize as same size integer and check for error
  if(!Read(temp))
    return false;

  //Special copy from integer to floating point
  memcpy(&val, &temp, sizeof(val));
  return true;
}

bool HCCell::Write(float val)
{
  uint32_t temp;

  //Special copy from floating point to integer
  memcpy(&temp, &val, sizeof(val));

  //Delegate to integer version of this method
  return Write(temp);
}

bool HCCell::Read(double& val)
{
  uint64_t temp;

  //Deserialize as same size integer and check for error
  if(!Read(temp))
    return false;

  //Special copy from integer to floating point
  memcpy(&val, &temp, sizeof(val));
  return true;
}

bool HCCell::Write(double val)
{
  uint64_t temp;

  //Special copy from floating point to integer
  memcpy(&temp, &val, sizeof(val));

  //Delegate to integer version of this method
  return Write(temp);
}

bool HCCell::Read(float& val0, float& val1)
{
  uint32_t temp0;
  uint32_t temp1;

  //Deserialize as same size integer and check for error
  if(!Read(temp0))
    return false;

  if(!Read(temp1))
    return false;

  //Special copy from integer to floating point
  memcpy(&val0, &temp0, sizeof(val0));
  memcpy(&val1, &temp1, sizeof(val1));
  return true;
}

bool HCCell::Write(float val0, float val1)
{
  uint32_t temp0;
  uint32_t temp1;

  //Special copy from floating point to integer
  memcpy(&temp0, &val0, sizeof(val0));
  memcpy(&temp1, &val1, sizeof(val1));

  //Delegate to integer version of this method
  if(!Write(temp0))
    return false;

  if(!Write(temp1))
    return false;

  return true;
}

bool HCCell::Read(double& val0, double& val1)
{
  uint64_t temp0;
  uint64_t temp1;

  //Deserialize as same size integer and check for error
  if(!Read(temp0))
    return false;

  if(!Read(temp1))
    return false;

  //Special copy from integer to floating point
  memcpy(&val0, &temp0, sizeof(val0));
  memcpy(&val1, &temp1, sizeof(val1));
  return true;
}

bool HCCell::Write(double val0, double val1)
{
  uint64_t temp0;
  uint64_t temp1;

  //Special copy from floating point to integer
  memcpy(&temp0, &val0, sizeof(val0));
  memcpy(&temp1, &val1, sizeof(val1));

  //Delegate to integer version of this method
  if(!Write(temp0))
    return false;

  if(!Write(temp1))
    return false;

  return true;
}

bool HCCell::Read(float& val0, float& val1, float& val2)
{
  uint32_t temp0;
  uint32_t temp1;
  uint32_t temp2;

  //Deserialize as same size integer and check for error
  if(!Read(temp0))
    return false;

  if(!Read(temp1))
    return false;

  if(!Read(temp2))
    return false;

  //Special copy from integer to floating point
  memcpy(&val0, &temp0, sizeof(val0));
  memcpy(&val1, &temp1, sizeof(val1));
  memcpy(&val2, &temp2, sizeof(val2));
  return true;
}

bool HCCell::Write(float val0, float val1, float val2)
{
  uint32_t temp0;
  uint32_t temp1;
  uint32_t temp2;

  //Special copy from floating point to integer
  memcpy(&temp0, &val0, sizeof(val0));
  memcpy(&temp1, &val1, sizeof(val1));
  memcpy(&temp2, &val2, sizeof(val2));

  //Delegate to integer version of this method
  if(!Write(temp0))
    return false;

  if(!Write(temp1))
    return false;

  if(!Write(temp2))
    return false;

  return true;
}

bool HCCell::Read(double& val0, double& val1, double& val2)
{
  uint64_t temp0;
  uint64_t temp1;
  uint64_t temp2;

  //Deserialize as same size integer and check for error
  if(!Read(temp0))
    return false;

  if(!Read(temp1))
    return false;

  if(!Read(temp2))
    return false;

  //Special copy from integer to floating point
  memcpy(&val0, &temp0, sizeof(val0));
  memcpy(&val1, &temp1, sizeof(val1));
  memcpy(&val2, &temp2, sizeof(val2));
  return true;
}

bool HCCell::Write(double val0, double val1, double val2)
{
  uint64_t temp0;
  uint64_t temp1;
  uint64_t temp2;

  //Special copy from floating point to integer
  memcpy(&temp0, &val0, sizeof(val0));
  memcpy(&temp1, &val1, sizeof(val1));
  memcpy(&temp2, &val2, sizeof(val2));

  //Delegate to integer version of this method
  if(!Write(temp0))
    return false;

  if(!Write(temp1))
    return false;

  if(!Write(temp2))
    return false;

  return true;
}

void HCCell::Print(const string& extra)
{
  uint32_t i;

  //Print common info
  cout << extra << "Cell: OpCode=" << (uint16_t)_opcode << "=";

  //Print info dependent on cell opcode
  switch(_opcode)
  {
  case OPCODE_CALL_CMD:
    cout << "Call Cmd";
    break;
  case OPCODE_CALL_STS:
    cout << "Call Sts";
    break;
  case OPCODE_GET_CMD:
    cout << "Get Cmd";
    break;
  case OPCODE_GET_STS:
    cout << "Get Sts";
    break;
  case OPCODE_SET_CMD:
    cout << "Set Cmd";
    break;
  case OPCODE_SET_STS:
    cout << "Set Sts";
    break;
  case OPCODE_ICALL_CMD:
    cout << "ICall Cmd";
    break;
  case OPCODE_ICALL_STS:
    cout << "ICall Sts";
    break;
  case OPCODE_IGET_CMD:
    cout << "IGet Cmd";
    break;
  case OPCODE_IGET_STS:
    cout << "IGet Sts";
    break;
  case OPCODE_ISET_CMD:
    cout << "ISet Cmd";
    break;
  case OPCODE_ISET_STS:
    cout << "ISet Sts";
    break;
  case OPCODE_ADD_CMD:
    cout << "Add Cmd";
    break;
  case OPCODE_ADD_STS:
    cout << "Add Sts";
    break;
  case OPCODE_SUB_CMD:
    cout << "Sub Cmd";
    break;
  case OPCODE_SUB_STS:
    cout << "Sub Sts";
    break;
  case OPCODE_READ_CMD:
    cout << "Read Cmd";
    break;
  case OPCODE_READ_STS:
    cout << "Read Sts";
    break;
  case OPCODE_WRITE_CMD:
    cout << "Write Cmd";
    break;
  case OPCODE_WRITE_STS:
    cout << "Write Sts";
    break;
  default:
    cout << "Unknown";
    break;
  }

  //Print payload
  cout << ", Payload=";
  for(i=0; i<_payloadlength; i++)
    cout << hex << setw(2) << setfill('0') << (uint16_t)_payload[i] << dec << " ";
  cout << "\n";
}
