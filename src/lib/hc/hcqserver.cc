// HC query server
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
#include "hcqserver.hh"
#include "hcutility.hh"
#include "str.hh"
#include <cassert>
#include <inttypes.h>
#include <string.h>
#include <string>

using namespace std;

HCQServer::HCQServer(Device *lowdev, HCContainer *top)
{
  //Assert valid arguments
  assert((lowdev != 0) && (top != 0));

  //Initialize member variables
  _lowdev = lowdev;
  _top = top;
  _readcount = 0;
  memset(_readbuf, 0, sizeof(_readbuf));
  _readind = 0;
  memset(_writebuf, 0, sizeof(_writebuf));
  _writeind = 0;

  //Create and start control thread
  _ctlthread = new Thread<HCQServer>(this, &HCQServer::CtlThread);
  _ctlthread->Start();
}

HCQServer::~HCQServer()
{
  //Cleanup
  delete _ctlthread;
}

bool HCQServer::NextReadCharEquals(char ch)
{
  //Check for overflow
  if(_readind >= _readcount)
    return false;

  //Check for matching character
  if(_readbuf[_readind++] == ch)
    return true;

  //No match
  return false;
}

bool HCQServer::ReadField(char termchar, char *field, uint32_t fieldsize)
{
  char ch;
  uint32_t i;
  uint32_t maxchars;

  //Assert valid arguments
  assert((field != 0) && (fieldsize > 1));

  //Initialize field to zero length string
  field[0] = '\0';

  //Leave space for null terminator
  maxchars = fieldsize - 1;

  //Read field
  for(i=0; i<maxchars; i++)
  {
    //Check for overflow
    if(_readind >= _readcount)
      return false;

    //Get next character from buffer
    ch = _readbuf[_readind++];

    //Check for terminal character
    if(ch == termchar)
      return true;

    //Update field
    field[i] = ch;
    field[i+1] = '\0';
  }

  //Check for overflow
  if(_readind >= _readcount)
    return false;

  //Check for terminal character
  if(_readbuf[_readind++] == termchar)
    return true;

  //No terminal character found
  return false;
}

bool HCQServer::WriteChar(char ch)
{
  //Check for overflow
  if(_writeind >= sizeof(_writebuf) - 1)
    return false;

  //Write character to write buffer and null terminate
  _writebuf[_writeind++] = ch;
  _writebuf[_writeind] = '\0';

  //Success
  return true;
}

bool HCQServer::WriteString(const char *str)
{
  uint32_t len;

  //Assert valid arguments
  assert(str != 0);

  //Get string length
  len = strlen(str);

  //Check for overflow
  if(len >= sizeof(_writebuf) - _writeind)
    return false;

  //Write string to write buffer
  strcpy(_writebuf + _writeind, str);

  //Update write index
  _writeind += len;

  //Success
  return true;
}

bool HCQServer::WriteStringQuote(const char *str)
{
  uint32_t len;

  //Assert valid arguments
  assert(str != 0);

  //Get string length
  len = strlen(str);

  //Check for overflow
  if(len >= sizeof(_writebuf) - _writeind - 2)
    return false;

  //Write opening quote to write buffer
  _writebuf[_writeind++] = '"';

  //Write string to write buffer
  strcpy(_writebuf + _writeind, str);

  //Update write index
  _writeind += len;

  //Write closing quote to write buffer
  _writebuf[_writeind++] = '"';

  //Success
  return true;
}

bool HCQServer::ProcessCallCell(void)
{
  char pname[100];
  HCParameter *param;
  int err;

  //Check for next character not parameter name opening quote
  if(!NextReadCharEquals('"'))
    return false;

  //Read parameter name
  if(!ReadField('"', pname, sizeof(pname)))
    return false;

  //Check for next character not cell closing bracket
  if(!NextReadCharEquals(']'))
    return false;

  //Get parameter
  if((param = HCUtility::GetParam(pname, _top)) == 0)
    return false;

  //Call parameter
  err = param->Call();

  //Write parameter name to outbound message
  if(!WriteStringQuote(pname) || !WriteChar(','))
    return false;

  //Write to outbound message
  if(!WriteStringQuote(ErrToString(err).c_str()) || !WriteChar(']'))
    return false;

  //Success
  return true;
}

bool HCQServer::ProcessGetCell(void)
{
  char pname[100];
  HCParameter *param;
  string pval;
  int err;

  //Check for next character not parameter name opening quote
  if(!NextReadCharEquals('"'))
    return false;

  //Read parameter name
  if(!ReadField('"', pname, sizeof(pname)))
    return false;

  //Check for next character not cell closing bracket
  if(!NextReadCharEquals(']'))
    return false;

  //Get parameter
  if((param = HCUtility::GetParam(pname, _top)) == 0)
    return false;

  //Get parameter value string
  err = param->GetStr(pval);

  //Write parameter name to outbound message
  if(!WriteStringQuote(pname) || !WriteChar(','))
    return false;

  //Write to outbound message
  if(!WriteStringQuote(pval.c_str()) || !WriteChar(',') || !WriteStringQuote(ErrToString(err).c_str()) || !WriteChar(']'))
    return false;

  //Success
  return true;
}

bool HCQServer::ProcessSetCell(void)
{
  char pname[100];
  char pval[100];
  HCParameter *param;
  int err;

  //Check for next character not parameter name opening quote
  if(!NextReadCharEquals('"'))
    return false;

  //Read parameter name
  if(!ReadField('"', pname, sizeof(pname)))
    return false;

  //Check for next character not comma
  if(!NextReadCharEquals(','))
    return false;

  //Check for next character not parameter value opening quote
  if(!NextReadCharEquals('"'))
    return false;

  //Read parameter value
  if(!ReadField('"', pval, sizeof(pval)))
    return false;

  //Check for next character not cell closing bracket
  if(!NextReadCharEquals(']'))
    return false;

  //Get parameter
  if((param = HCUtility::GetParam(pname, _top)) == 0)
    return false;

  //Set parameter value string
  err = param->SetStr(pval);

  //Write parameter name to outbound message
  if(!WriteStringQuote(pname) || !WriteChar(','))
    return false;

  //Write to outbound message
  if(!WriteStringQuote(ErrToString(err).c_str()) || !WriteChar(']'))
    return false;

  //Success
  return true;
}

bool HCQServer::ProcessICallCell(void)
{
  char pname[100];
  char eidstr[11];
  uint32_t eid;
  HCParameter *param;
  int err;

  //Check for next character not parameter name opening quote
  if(!NextReadCharEquals('"'))
    return false;

  //Read parameter name
  if(!ReadField('"', pname, sizeof(pname)))
    return false;

  //Check for next character not comma
  if(!NextReadCharEquals(','))
    return false;

  //Read element identifier
  if(!ReadField(']', eidstr, sizeof(eidstr)))
    return false;

  //Convert element identifier to a number
  if(!StringConvert(eidstr, eid))
    return false;

  //Get parameter
  if((param = HCUtility::GetParam(pname, _top)) == 0)
    return false;

  //ICall parameter
  err = param->CallTbl(eid);

  //Write parameter name to outbound message
  if(!WriteStringQuote(pname) || !WriteChar(','))
    return false;

  //Write element identifier to outbound message
  if(!WriteString(eidstr) || !WriteChar(','))
    return false;

  //Write to outbound message
  if(!WriteStringQuote(ErrToString(err).c_str()) || !WriteChar(']'))
    return false;

  //Success
  return true;
}

bool HCQServer::ProcessIGetCell(void)
{
  char pname[100];
  char eidstr[11];
  uint32_t eid;
  HCParameter *param;
  string pval;
  int err;

  //Check for next character not parameter name opening quote
  if(!NextReadCharEquals('"'))
    return false;

  //Read parameter name
  if(!ReadField('"', pname, sizeof(pname)))
    return false;

  //Check for next character not comma
  if(!NextReadCharEquals(','))
    return false;

  //Read element identifier
  if(!ReadField(']', eidstr, sizeof(eidstr)))
    return false;

  //Convert element identifier to a number
  if(!StringConvert(eidstr, eid))
    return false;

  //Get parameter
  if((param = HCUtility::GetParam(pname, _top)) == 0)
    return false;

  //Get parameter table string value
  err = param->GetStrTbl(eid, pval);

  //Write parameter name to outbound message
  if(!WriteStringQuote(pname) || !WriteChar(','))
    return false;

  //Write element identifier to outbound message
  if(!WriteString(eidstr) || !WriteChar(','))
    return false;

  //Write to outbound message
  if(!WriteStringQuote(pval.c_str()) || !WriteChar(',') || !WriteStringQuote(ErrToString(err).c_str()) || !WriteChar(']'))
    return false;

  //Success
  return true;
}

bool HCQServer::ProcessISetCell(void)
{
  char pname[100];
  char eidstr[11];
  char pval[100];
  uint32_t eid;
  HCParameter *param;
  int err;

  //Check for next character not parameter name opening quote
  if(!NextReadCharEquals('"'))
    return false;

  //Read parameter name
  if(!ReadField('"', pname, sizeof(pname)))
    return false;

  //Check for next character not comma
  if(!NextReadCharEquals(','))
    return false;

  //Read element identifier
  if(!ReadField(',', eidstr, sizeof(eidstr)))
    return false;

  //Convert element identifier to a number
  if(!StringConvert(eidstr, eid))
    return false;

  //Check for next character not parameter value opening quote
  if(!NextReadCharEquals('"'))
    return false;

  //Read parameter value
  if(!ReadField('"', pval, sizeof(pval)))
    return false;

  //Check for next character not cell ending bracket
  if(!NextReadCharEquals(']'))
    return false;

  //Get parameter
  if((param = HCUtility::GetParam(pname, _top)) == 0)
    return false;

  //Set parameter table string value
  err = param->SetStrTbl(eid, pval);

  //Write parameter name to outbound message
  if(!WriteStringQuote(pname) || !WriteChar(','))
    return false;

  //Write element identifier to outbound message
  if(!WriteString(eidstr) || !WriteChar(','))
    return false;

  //Write to outbound message
  if(!WriteStringQuote(ErrToString(err).c_str()) || !WriteChar(']'))
    return false;

  //Success
  return true;
}

bool HCQServer::ProcessAddCell(void)
{
  char pname[100];
  char pval[100];
  HCParameter *param;
  int err;

  //Check for next character not parameter name opening quote
  if(!NextReadCharEquals('"'))
    return false;

  //Read parameter name
  if(!ReadField('"', pname, sizeof(pname)))
    return false;

  //Check for next character not comma
  if(!NextReadCharEquals(','))
    return false;

  //Check for next character not parameter value opening quote
  if(!NextReadCharEquals('"'))
    return false;

  //Read parameter value
  if(!ReadField('"', pval, sizeof(pval)))
    return false;

  //Check for next character not cell closing bracket
  if(!NextReadCharEquals(']'))
    return false;

  //Get parameter
  if((param = HCUtility::GetParam(pname, _top)) == 0)
    return false;

  //Add parameter value string
  err = param->AddStr(pval);

  //Write parameter name to outbound message
  if(!WriteStringQuote(pname) || !WriteChar(','))
    return false;

  //Write to outbound message
  if(!WriteStringQuote(ErrToString(err).c_str()) || !WriteChar(']'))
    return false;

  //Success
  return true;
}

bool HCQServer::ProcessSubCell(void)
{
  char pname[100];
  char pval[100];
  HCParameter *param;
  int err;

  //Check for next character not parameter name opening quote
  if(!NextReadCharEquals('"'))
    return false;

  //Read parameter name
  if(!ReadField('"', pname, sizeof(pname)))
    return false;

  //Check for next character not comma
  if(!NextReadCharEquals(','))
    return false;

  //Check for next character not parameter value opening quote
  if(!NextReadCharEquals('"'))
    return false;

  //Read parameter value
  if(!ReadField('"', pval, sizeof(pval)))
    return false;

  //Check for next character not cell closing bracket
  if(!NextReadCharEquals(']'))
    return false;

  //Get parameter
  if((param = HCUtility::GetParam(pname, _top)) == 0)
    return false;

  //Subtract parameter value string
  err = param->SubStr(pval);

  //Write parameter name to outbound message
  if(!WriteStringQuote(pname) || !WriteChar(','))
    return false;

  //Write to outbound message
  if(!WriteStringQuote(ErrToString(err).c_str()) || !WriteChar(']'))
    return false;

  //Success
  return true;
}

bool HCQServer::ProcessCell(void)
{
  char opcode[3];

  //Check for next read character not opcode opening quote
  if(!NextReadCharEquals('"'))
    return false;

  //Read opcode
  if(!ReadField('"', opcode, sizeof(opcode)))
    return false;

  //Check for next read character not comma
  if(!NextReadCharEquals(','))
    return false;

  //Write to outbound message
  if(!WriteString("[\"") || !WriteString(opcode) || !WriteString("\","))
    return false;

  //Process cells depending on opcode
  if(strcmp(opcode, "ca") == 0)
    return ProcessCallCell();
  else if(strcmp(opcode, "ge") == 0)
    return ProcessGetCell();
  else if(strcmp(opcode, "se") == 0)
    return ProcessSetCell();
  else if(strcmp(opcode, "ic") == 0)
    return ProcessICallCell();
  else if(strcmp(opcode, "ig") == 0)
    return ProcessIGetCell();
  else if(strcmp(opcode, "is") == 0)
    return ProcessISetCell();
  else if(strcmp(opcode, "ad") == 0)
    return ProcessAddCell();
  else if(strcmp(opcode, "su") == 0)
    return ProcessSubCell();

  //Unrecognized opcode
  return false;
}

bool HCQServer::ProcessMessage(void)
{
  char ch;
  char xactstr[3];
  uint8_t xact;

  //Reset read and write indices
  _readind = 0;
  _writeind = 0;

  //Check for overflow
  if((_readind + 1) >= _readcount)
    return false;

  //Check for next read character not message opening bracket
  if(!NextReadCharEquals('['))
    return false;

  //Read transaction number
  if(!ReadField(',', xactstr, sizeof(xactstr)))
    return false;

  //Convert transaction number string to a number (ensure it's an actual number)
  if(!StringConvert(xactstr, xact))
    return false;

  //Write to outbound message
  if(!WriteChar('[') || !WriteString(xactstr) || !WriteChar(','))
    return false;

  //Process all cells
  while(true)
  {
    //Check for next read character not cell opening bracket
    if(!NextReadCharEquals('['))
      return false;

    //Process cell
    if(!ProcessCell())
      return false;

    //Check for overflow
    if(_readind >= _readcount)
      return false;

    //Get next character from read buffer
    ch = _readbuf[_readind++];

    //Check for message ending bracket
    if(ch == ']')
      break;

    //Must be a comma
    if(ch != ',')
      return false;

    //Write to outbound message
    if(!WriteChar(','))
      return false;
  }

  //Check for not all characters consumed
  if(_readind != _readcount)
    return false;

  //Write to outbound message
  if(!WriteChar(']'))
    return false;

  //Success
  return true;
}

void HCQServer::CtlThread(void)
{
  //Go forever
  while(true)
  {
    //Read inbound message from device (leave room for null terminator)
    if((_readcount = _lowdev->Read(_readbuf, sizeof(_readbuf)-1)) == 0)
    {
      //Sleep a while to prevent starving other threads
      ThreadSleep(1000000);

      //Ignore rest of loop
      continue;
    }

    //Ensure null termination
    _readbuf[_readcount] = '\0';

    //Process message
    if(!ProcessMessage())
      continue;

    //Write outbound message to device
    _lowdev->Write(_writebuf, _writeind);
  }
}
