// HC server
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

#include "crc.hh"
#include "error.hh"
#include "hcboolean.hh"
#include "hccall.hh"
#include "hcfile.hh"
#include "hcinteger.hh"
#include "hcserver.hh"
#include "hcstring.hh"
#include "hcutility.hh"
#include <cassert>
#include <iostream>
#include <stdio.h>

using namespace std;

static const HCBooleanEnum Offon[] =
{
  HCBooleanEnum(0, "Off"),
  HCBooleanEnum(1, "On"),
  HCBooleanEnum()
};

HCServer::HCServer(Device *lowdev, HCContainer *top, const string &name, const string &version, uint32_t pidmax)
{
  HCContainer *cont;
  uint32_t i;
  HCParameter *param;

  //Assert valid arguments
  assert((lowdev != 0) && (top != 0) && (pidmax <= HCServer::PID_MAX));

  //Initialize low device and top container
  _lowdev = lowdev;
  _top = top;

  //Copy name, version and information file name
  _name = name;
  _version = version;
  _infofilename = ".server-";
  _infofilename += name;
  _infofilename += ".xml";

  //Initialize PID top and maximum PID count
  _pidtop = 0;
  _pidmax = pidmax;

  //Allocate memory for the parameter array
  _params = new HCParameter *[_pidmax];

  //Zero out parameter array
  for(i=0; i<_pidmax; i++)
    _params[i] = 0;

  //Clear started flag
  _started = false;

  //Create inbound and outbound message and cell storage
  _imsg = new HCMessage();
  _icell = new HCCell();
  _omsg = new HCMessage();
  _ocell = new HCCell();

  //Initialize debug flag and counts
  _debug = false;
  _senderrcnt = 0;
  _recverrcnt = 0;
  _deserrcnt = 0;
  _cellerrcnt = 0;
  _opcodeerrcnt = 0;
  _piderrcnt = 0;
  _interrcnt = 0;
  _goodxactcnt = 0;

  //Create server container and add to top container
  cont = new HCContainer(".server");
  top->Add(cont);

  //Add special parameters to server and container
  param = new HCStr<HCServer>("name", this, &HCServer::GetName, 0);
  Add(param);
  cont->Add(param);
  param = new HCStr<HCServer>("version", this, &HCServer::GetVersion, 0);
  Add(param);
  cont->Add(param);
  param = new HCUns32<HCServer>("infofilecrc", this, &HCServer::GetInfoFileCRC, 0);
  Add(param);
  cont->Add(param);
  param = new HCFile<HCServer>("infofile", this, &HCServer::ReadInfoFile, 0);
  Add(param);
  cont->Add(param);

  //Add parameters to server container
  cont->Add(new HCBoolean<HCServer>("debug", this, &HCServer::GetDebug, &HCServer::SetDebug, Offon));
  cont->Add(new HCUns32<HCServer>("senderrcnt", this, &HCServer::GetSendErrCnt, 0));
  cont->Add(new HCUns32<HCServer>("recverrcnt", this, &HCServer::GetRecvErrCnt, 0));
  cont->Add(new HCUns32<HCServer>("deserrcnt", this, &HCServer::GetDesErrCnt, 0));
  cont->Add(new HCUns32<HCServer>("cellerrcnt", this, &HCServer::GetCellErrCnt, 0));
  cont->Add(new HCUns32<HCServer>("opcodeerrcnt", this, &HCServer::GetOpCodeErrCnt, 0));
  cont->Add(new HCUns32<HCServer>("piderrcnt", this, &HCServer::GetPIDErrCnt, 0));
  cont->Add(new HCUns32<HCServer>("interrcnt", this, &HCServer::GetIntErrCnt, 0));
  cont->Add(new HCUns32<HCServer>("goodxactcnt", this, &HCServer::GetGoodXactCnt, 0));

  //Create control thread
  _ctlthread = new Thread<HCServer>(this, &HCServer::CtlThread);
}

HCServer::~HCServer()
{
  //Cleanup
  delete _ctlthread;
  delete _imsg;
  delete _icell;
  delete _omsg;
  delete _ocell;
  delete[] _params;
}

HCParameter *HCServer::GetParam(uint16_t pid)
{
  //Check for parameter id out of bounds
  if(pid >= _pidmax)
  {
    cout << __FILE__ << ' ' << __LINE__ << " - Parameter id out of bounds (" << pid << " >= " << _pidmax << ')' << "\n";
    return 0;
  }

  //Return parameter
  return _params[pid];
}

void HCServer::Add(HCParameter *param)
{
  //Assert valid arguments
  assert(param != 0);

  //Check for parameter array full
  if(_pidtop >= _pidmax)
  {
    cout << __FILE__ << ' ' << __LINE__ << " - Parameter top (" << _pidtop << ") is at max (" << _pidmax << ')' << "\n";
    return;
  }

  //Check for already started
  if(_started)
    return;

  //Add parameter to parameter array
  _params[_pidtop++] = param;
}

void HCServer::Start(void)
{
  //Save to XML file
  SaveInfo();

  //Set started flag
  _started = true;

  //Start the control thread
  _ctlthread->Start();
}

int HCServer::GetName(string &val)
{
  //Get name
  val = _name;
  return ERR_NONE;
}

int HCServer::GetVersion(string &val)
{
  //Get version
  val = _version;
  return ERR_NONE;
}

int HCServer::GetInfoFileCRC(uint32_t &val)
{
  //Calculate CRC of info file
  val = CRC32File(_infofilename);
  return ERR_NONE;
}

int HCServer::ReadInfoFile(uint32_t offset, uint8_t *val, uint16_t maxlen, uint16_t &len)
{
  FILE *file;

  //Open info file and check for error
  if((file = fopen(_infofilename.c_str(), "r")) == NULL)
  {
    //Indicate zero bytes read
    len = 0;
    return ERR_ACCESS;
  }

  //Seek to offset
  if(fseek(file, offset, SEEK_SET) < 0)
  {
    //Close file
    fclose(file);

    //Indicate zero bytes read
    len = 0;
    return ERR_RANGE;
  }

  //Read file
  len = fread(val, 1, maxlen, file);

  //Close file
  fclose(file);
  return ERR_NONE;
}

int HCServer::GetDebug(bool &val)
{
  //Get debug flag
  val = _debug;

  return ERR_NONE;
}

int HCServer::SetDebug(bool val)
{
  //Set debug flag
  _debug = val;

  return ERR_NONE;
}

int HCServer::GetSendErrCnt(uint32_t &val)
{
  //Get count value
  val = _senderrcnt;

  return ERR_NONE;
}

int HCServer::GetRecvErrCnt(uint32_t &val)
{
  //Get count value
  val = _recverrcnt;

  return ERR_NONE;
}

int HCServer::GetDesErrCnt(uint32_t &val)
{
  //Get count value
  val = _deserrcnt;

  return ERR_NONE;
}

int HCServer::GetCellErrCnt(uint32_t &val)
{
  //Get count value
  val = _cellerrcnt;

  return ERR_NONE;
}

int HCServer::GetOpCodeErrCnt(uint32_t &val)
{
  //Get count value
  val = _opcodeerrcnt;

  return ERR_NONE;
}

int HCServer::GetPIDErrCnt(uint32_t &val)
{
  //Get count value
  val = _piderrcnt;

  return ERR_NONE;
}

int HCServer::GetIntErrCnt(uint32_t &val)
{
  //Get count value
  val = _interrcnt;

  return ERR_NONE;
}

int HCServer::GetGoodXactCnt(uint32_t &val)
{
  //Get count value
  val = _goodxactcnt;

  return ERR_NONE;
}

void HCServer::SaveInfo(void)
{
  ofstream file;
  HCParameter *param;
  HCContainer *cont;
  uint16_t pid;

  //Open information file
  file.open(_infofilename.c_str());

  //Check for error
  if(!file.is_open())
  {
    cout << __FILE__ << ' ' << __LINE__ << " - Error opening file (" << _infofilename << ')' << "\n";
    return;
  }

  //Header
  file << "<server>" << "\n";
  file << "  <name>" << _name << "</name>" << "\n";
  file << "  <version>" << _version << "</version>" << "\n";

  //Save child parameters if contained in PID table
  for(param=_top->GetFirstSubParam(); param!=0; param=param->GetNext())
    if(ParamToPID(param, &pid))
      param->SaveInfo(file, 2, pid);

  //Save child containers
  for(cont=_top->GetFirstSubCont(); cont!=0; cont=cont->GetNext())
    SaveInfo(file, 2, cont);

  //Footer
  file << "</server>" << "\n";

  //Close information file
  file.close();
}

void HCServer::SaveInfo(ofstream &file, uint32_t indent, HCContainer *startcont)
{
  HCParameter *param;
  HCContainer *cont;
  uint16_t pid;

  //Assert valid arguments
  assert(startcont != 0);

  //Header
  file << string(indent, ' ') << "<cont>" << "\n";
  file << string(indent, ' ') << "  <name>" << startcont->GetName() << "</name>" << "\n";

  //Save child parameters if contained in PID table
  for(param=startcont->GetFirstSubParam(); param!=0; param=param->GetNext())
    if(ParamToPID(param, &pid))
      param->SaveInfo(file, indent + 2, pid);

  //Save child containers
  for(cont=startcont->GetFirstSubCont(); cont!=0; cont=cont->GetNext())
    SaveInfo(file, indent + 2, cont);

  //Footer
  file << string(indent, ' ') << "</cont>" << "\n";
}

bool HCServer::ParamToPID(HCParameter *param, uint16_t *pid)
{
  uint32_t i;

  //Assert valid arguments
  assert((param != 0) && (pid != 0));

  //Find matching parameter
  for(i=0; i<_pidmax; i++)
  {
    if(_params[i] == param)
    {
      *pid = i;
      return true;
    }
  }

  return false;
}

void HCServer::CallCmdHandler(void)
{
  uint16_t pid;
  HCParameter *param;

  //Reset outbound cell
  _ocell->Reset(HCCell::OPCODE_CALL_STS);

  //Read PID from inbound cell and check for error
  if(!_icell->Read(pid))
  {
    //Increment deserialization error count
    _deserrcnt++;

    //Stop processing
    return;
  }

  //Get a pointer to parameter and check for error
  if((pid >= _pidmax) || ((param = _params[pid]) == 0))
  {
    //Increment PID error count
    _piderrcnt++;

    //Write PID to outbound cell and check for error
    if(!_ocell->Write(pid))
      return;

    //Write PID error code to outbound cell and check for error
    if(!_ocell->Write((int8_t)ERR_PID))
      return;

    //Write outbound cell to message
    _omsg->Write(_ocell);

    //Stop processing
    return;
  }

  //Write PID to outbound cell and check for error
  if(!_ocell->Write(pid))
  {
    //Increment internal error count
    _interrcnt++;

    //Stop processing
    return;
  }

  //Call parameter call cell function and check for error
  if(!param->CallCell(_icell, _ocell))
  {
    //Increment internal error count
    _interrcnt++;

    //Stop processing
    return;
  }

  //Write outbound cell to message
  _omsg->Write(_ocell);
}

void HCServer::GetCmdHandler(void)
{
  uint16_t pid;
  HCParameter *param;

  //Reset outbound cell
  _ocell->Reset(HCCell::OPCODE_GET_STS);

  //Read PID from inbound cell and check for error
  if(!_icell->Read(pid))
  {
    //Increment deserialization error count
    _deserrcnt++;

    //Stop processing
    return;
  }

  //Get a pointer to parameter and check for error
  if((pid >= _pidmax) || ((param = _params[pid]) == 0))
  {
    //Increment PID error count
    _piderrcnt++;

    //Write PID to outbound cell and check for error
    if(!_ocell->Write(pid))
      return;

    //Handle PID error for get transaction
    if(!HCParameter::HandleGetPIDError(_icell, _ocell))
      return;

    //Write outbound cell to message
    _omsg->Write(_ocell);

    //Stop processing
    return;
  }

  //Write PID to outbound cell and check for error
  if(!_ocell->Write(pid))
  {
    //Increment internal error count
    _interrcnt++;

    //Stop processing
    return;
  }

  //Call parameter get cell function and check for error
  if(!param->GetCell(_icell, _ocell))
  {
    //Increment internal error count
    _interrcnt++;

    //Stop processing
    return;
  }

  //Write outbound cell to message
  _omsg->Write(_ocell);
}

void HCServer::SetCmdHandler(void)
{
  uint16_t pid;
  HCParameter *param;

  //Reset outbound cell
  _ocell->Reset(HCCell::OPCODE_SET_STS);

  //Read PID from inbound cell and check for error
  if(!_icell->Read(pid))
  {
    //Increment deserialization error count
    _deserrcnt++;

    //Stop processing
    return;
  }

  //Get a pointer to parameter and check for error
  if((pid >= _pidmax) || ((param = _params[pid]) == 0))
  {
    //Increment PID error count
    _piderrcnt++;

    //Write PID to outbound cell and check for error
    if(!_ocell->Write(pid))
      return;

    //Handle PID error for set transaction
    if(!HCParameter::HandleSetPIDError(_icell, _ocell))
      return;

    //Write outbound cell to message
    _omsg->Write(_ocell);

    //Stop processing
    return;
  }

  //Write PID to outbound cell and check for error
  if(!_ocell->Write(pid))
  {
    //Increment internal error count
    _interrcnt++;

    //Stop processing
    return;
  }

  //Call parameter set cell function and check for error
  if(!param->SetCell(_icell, _ocell))
  {
    //Increment internal error count
    _interrcnt++;

    //Stop processing
    return;
  }

  //Write outbound cell to message
  _omsg->Write(_ocell);
}

void HCServer::ICallCmdHandler(void)
{
  uint16_t pid;
  uint32_t eid;
  HCParameter *param;

  //Reset outbound cell
  _ocell->Reset(HCCell::OPCODE_ICALL_STS);

  //Read PID from inbound cell and check for error
  if(!_icell->Read(pid))
  {
    //Increment deserialization error count
    _deserrcnt++;

    //Stop processing
    return;
  }

  //Read EID from inbound cell and check for error
  if(!_icell->Read(eid))
  {
    //Increment deserialization error count
    _deserrcnt++;

    //Stop processing
    return;
  }

  //Get a pointer to parameter and check for error
  if((pid >= _pidmax) || ((param = _params[pid]) == 0))
  {
    //Increment PID error count
    _piderrcnt++;

    //Write PID to outbound cell and check for error
    if(!_ocell->Write(pid))
      return;

    //Write EID to outbound cell and check for error
    if(!_ocell->Write(eid))
      return;

    //Write PID error code to outbound cell and check for error
    if(!_ocell->Write((int8_t)ERR_PID))
      return;

    //Write outbound cell to message
    _omsg->Write(_ocell);

    //Stop processing
    return;
  }

  //Write PID to outbound cell and check for error
  if(!_ocell->Write(pid))
  {
    //Increment internal error count
    _interrcnt++;

    //Stop processing
    return;
  }

  //Write EID to outbound cell and check for error
  if(!_ocell->Write(eid))
  {
    //Increment internal error count
    _interrcnt++;

    //Stop processing
    return;
  }

  //Call parameter call cell table function and check for error
  if(!param->CallCellTbl(eid, _icell, _ocell))
  {
    //Increment internal error count
    _interrcnt++;

    //Stop processing
    return;
  }

  //Write outbound cell to message
  _omsg->Write(_ocell);
}

void HCServer::IGetCmdHandler(void)
{
  uint16_t pid;
  uint32_t eid;
  HCParameter *param;

  //Reset outbound cell
  _ocell->Reset(HCCell::OPCODE_IGET_STS);

  //Read PID from inbound cell and check for error
  if(!_icell->Read(pid))
  {
    //Increment deserialization error count
    _deserrcnt++;

    //Stop processing
    return;
  }

  //Read EID from inbound cell and check for error
  if(!_icell->Read(eid))
  {
    //Increment deserialization error count
    _deserrcnt++;

    //Stop processing
    return;
  }

  //Get a pointer to parameter and check for error
  if((pid >= _pidmax) || ((param = _params[pid]) == 0))
  {
    //Increment PID error count
    _piderrcnt++;

    //Write PID to outbound cell and check for error
    if(!_ocell->Write(pid))
      return;

    //Write EID to outbound cell and check for error
    if(!_ocell->Write(eid))
      return;

    //Handle PID error for iget transaction same as get
    if(!HCParameter::HandleGetPIDError(_icell, _ocell))
      return;

    //Write outbound cell to message
    _omsg->Write(_ocell);

    //Stop processing
    return;
  }

  //Write PID to outbound cell and check for error
  if(!_ocell->Write(pid))
  {
    //Increment internal error count
    _interrcnt++;

    //Stop processing
    return;
  }

  //Write EID to outbound cell and check for error
  if(!_ocell->Write(eid))
  {
    //Increment internal error count
    _interrcnt++;

    //Stop processing
    return;
  }

  //Call parameter get cell function and check for error
  if(!param->GetCellTbl(eid, _icell, _ocell))
  {
    //Increment internal error count
    _interrcnt++;

    //Stop processing
    return;
  }

  //Write outbound cell to message
  _omsg->Write(_ocell);
}

void HCServer::ISetCmdHandler(void)
{
  uint16_t pid;
  uint32_t eid;
  HCParameter *param;

  //Reset outbound cell
  _ocell->Reset(HCCell::OPCODE_ISET_STS);

  //Read PID from inbound cell and check for error
  if(!_icell->Read(pid))
  {
    //Increment deserialization error count
    _deserrcnt++;

    //Stop processing
    return;
  }

  //Read EID from inbound cell and check for error
  if(!_icell->Read(eid))
  {
    //Increment deserialization error count
    _deserrcnt++;

    //Stop processing
    return;
  }

  //Get a pointer to parameter and check for error
  if((pid >= _pidmax) || ((param = _params[pid]) == 0))
  {
    //Increment PID error count
    _piderrcnt++;

    //Write PID to outbound cell and check for error
    if(!_ocell->Write(pid))
      return;

    //Write EID to outbound cell and check for error
    if(!_ocell->Write(eid))
      return;

    //Handle PID error for iset transaction same as set
    if(!HCParameter::HandleSetPIDError(_icell, _ocell))
      return;

    //Write outbound cell to message
    _omsg->Write(_ocell);

    //Stop processing
    return;
  }

  //Write PID to outbound cell and check for error
  if(!_ocell->Write(pid))
  {
    //Increment internal error count
    _interrcnt++;

    //Stop processing
    return;
  }

  //Write EID to outbound cell and check for error
  if(!_ocell->Write(eid))
  {
    //Increment internal error count
    _interrcnt++;

    //Stop processing
    return;
  }

  //Call parameter set cell function and check for error
  if(!param->SetCellTbl(eid, _icell, _ocell))
  {
    //Increment internal error count
    _interrcnt++;

    //Stop processing
    return;
  }

  //Write outbound cell to message
  _omsg->Write(_ocell);
}

void HCServer::AddCmdHandler(void)
{
  uint16_t pid;
  HCParameter *param;

  //Reset outbound cell
  _ocell->Reset(HCCell::OPCODE_ADD_STS);

  //Read PID from inbound cell and check for error
  if(!_icell->Read(pid))
  {
    //Increment deserialization error count
    _deserrcnt++;

    //Stop processing
    return;
  }

  //Get a pointer to parameter and check for error
  if((pid >= _pidmax) || ((param = _params[pid]) == 0))
  {
    //Increment PID error count
    _piderrcnt++;

    //Write PID to outbound cell and check for error
    if(!_ocell->Write(pid))
      return;

    //Handle PID error for add transaction same as set
    if(!HCParameter::HandleSetPIDError(_icell, _ocell))
      return;

    //Write outbound cell to message
    _omsg->Write(_ocell);

    //Stop processing
    return;
  }

  //Write PID to outbound cell and check for error
  if(!_ocell->Write(pid))
  {
    //Increment internal error count
    _interrcnt++;

    //Stop processing
    return;
  }

  //Call parameter add cell function and check for error
  if(!param->AddCell(_icell, _ocell))
  {
    //Increment internal error count
    _interrcnt++;

    //Stop processing
    return;
  }

  //Write outbound cell to message
  _omsg->Write(_ocell);
}

void HCServer::SubCmdHandler(void)
{
  uint16_t pid;
  HCParameter *param;

  //Reset outbound cell
  _ocell->Reset(HCCell::OPCODE_SUB_STS);

  //Read PID from inbound cell and check for error
  if(!_icell->Read(pid))
  {
    //Increment deserialization error count
    _deserrcnt++;

    //Stop processing
    return;
  }

  //Get a pointer to parameter and check for error
  if((pid >= _pidmax) || ((param = _params[pid]) == 0))
  {
    //Increment PID error count
    _piderrcnt++;

    //Write PID to outbound cell and check for error
    if(!_ocell->Write(pid))
      return;

    //Handle PID error for subtract transaction same as set
    if(!HCParameter::HandleSetPIDError(_icell, _ocell))
      return;

    //Write outbound cell to message
    _omsg->Write(_ocell);

    //Stop processing
    return;
  }

  //Write PID to outbound cell and check for error
  if(!_ocell->Write(pid))
  {
    //Increment internal error count
    _interrcnt++;

    //Stop processing
    return;
  }

  //Call parameter sub cell function and check for error
  if(!param->SubCell(_icell, _ocell))
  {
    //Increment internal error count
    _interrcnt++;

    //Stop processing
    return;
  }

  //Write outbound cell to message
  _omsg->Write(_ocell);
}

void HCServer::ReadCmdHandler(void)
{
  uint16_t pid;
  uint32_t offset;
  uint16_t maxlen;
  HCParameter *param;

  //Reset outbound cell
  _ocell->Reset(HCCell::OPCODE_READ_STS);

  //Read PID from inbound cell and check for error
  if(!_icell->Read(pid))
  {
    //Increment deserialization error count
    _deserrcnt++;

    //Stop processing
    return;
  }

  //Read offset from inbound cell and check for error
  if(!_icell->Read(offset))
  {
    //Increment deserialization error count
    _deserrcnt++;

    //Stop processing
    return;
  }

  //Read maximum length from inbound cell and check for error
  if(!_icell->Read(maxlen))
  {
    //Increment deserialization error count
    _deserrcnt++;

    //Stop processing
    return;
  }

  //Get a pointer to parameter and check for error
  if((pid >= _pidmax) || ((param = _params[pid]) == 0))
  {
    //Increment PID error count
    _piderrcnt++;

    //Write PID to outbound cell and check for error
    if(!_ocell->Write(pid))
      return;

    //Write offset to outbound cell and check for error
    if(!_ocell->Write(offset))
      return;

    //Handle PID error for read transaction same as get
    if(!HCParameter::HandleGetPIDError(_icell, _ocell))
      return;

    //Write outbound cell to message
    _omsg->Write(_ocell);

    //Stop processing
    return;
  }

  //Write PID to outbound cell and check for error
  if(!_ocell->Write(pid))
  {
    //Increment internal error count
    _interrcnt++;

    //Stop processing
    return;
  }

  //Write offset to outbound cell and check for error
  if(!_ocell->Write(offset))
  {
    //Increment internal error count
    _interrcnt++;

    //Stop processing
    return;
  }

  //Call parameter read cell function and check for error
  if(!param->ReadCell(offset, maxlen, _icell, _ocell))
  {
    //Increment internal error count
    _interrcnt++;

    //Stop processing
    return;
  }

  //Write outbound cell to message
  _omsg->Write(_ocell);
}

void HCServer::WriteCmdHandler(void)
{
  uint16_t pid;
  uint32_t offset;
  HCParameter *param;

  //Reset outbound cell
  _ocell->Reset(HCCell::OPCODE_WRITE_STS);

  //Read PID from inbound cell and check for error
  if(!_icell->Read(pid))
  {
    //Increment deserialization error count
    _deserrcnt++;

    //Stop processing
    return;
  }

  //Read offset from inbound cell and check for error
  if(!_icell->Read(offset))
  {
    //Increment deserialization error count
    _deserrcnt++;

    //Stop processing
    return;
  }

  //Get a pointer to parameter and check for error
  if((pid >= _pidmax) || ((param = _params[pid]) == 0))
  {
    //Increment PID error count
    _piderrcnt++;

    //Write PID to outbound cell and check for error
    if(!_ocell->Write(pid))
      return;

    //Write offset to outbound cell and check for error
    if(!_ocell->Write(offset))
      return;

    //Handle PID error for write transaction same as set
    if(!HCParameter::HandleSetPIDError(_icell, _ocell))
      return;

    //Write outbound cell to message
    _omsg->Write(_ocell);

    //Stop processing
    return;
  }

  //Write PID to outbound cell and check for error
  if(!_ocell->Write(pid))
  {
    //Increment internal error count
    _interrcnt++;

    //Stop processing
    return;
  }

  //Write offset to outbound cell and check for error
  if(!_ocell->Write(offset))
  {
    //Increment internal error count
    _interrcnt++;

    //Stop processing
    return;
  }

  //Call parameter set cell function and check for error
  if(!param->WriteCell(offset, _icell, _ocell))
  {
    //Increment internal error count
    _interrcnt++;

    //Stop processing
    return;
  }

  //Write outbound cell to message
  _omsg->Write(_ocell);
}

void HCServer::CtlThread(void)
{
  uint8_t opcode;

  //Go forever
  while(true)
  {
    //Receive inbound message and check for error
    if(_imsg->Recv(_lowdev) != ERR_NONE)
    {
      //Increment receive error count
      _recverrcnt++;

      //Ignore rest of loop
      continue;
    }

    //Print inbound message if requested
    if(_debug)
      _imsg->Print("Rx");

    //Reset outbound message
    _omsg->Reset(_imsg->GetTransaction());

    //Process all cells from inbound message
    while(_imsg->Read(_icell))
    {
      //Get opcode
      opcode = _icell->GetOpCode();

      //Process cells depending on opcode
      switch(opcode)
      {
      case HCCell::OPCODE_CALL_CMD:
        CallCmdHandler();
        break;
      case HCCell::OPCODE_GET_CMD:
        GetCmdHandler();
        break;
      case HCCell::OPCODE_SET_CMD:
        SetCmdHandler();
        break;
      case HCCell::OPCODE_ICALL_CMD:
        ICallCmdHandler();
        break;
      case HCCell::OPCODE_IGET_CMD:
        IGetCmdHandler();
        break;
      case HCCell::OPCODE_ISET_CMD:
        ISetCmdHandler();
        break;
      case HCCell::OPCODE_ADD_CMD:
        AddCmdHandler();
        break;
      case HCCell::OPCODE_SUB_CMD:
        SubCmdHandler();
        break;
      case HCCell::OPCODE_READ_CMD:
        ReadCmdHandler();
        break;
      case HCCell::OPCODE_WRITE_CMD:
        WriteCmdHandler();
        break;
      default:
        //Increment opcode error count
        _opcodeerrcnt++;

        //Ignore rest of loop
        continue;
      }
    }

    //Print outbound message if requested
    if(_debug)
      _omsg->Print("Tx");

    //Send outbound message and check for error
    if(_omsg->Send(_lowdev) != ERR_NONE)
    {
      //Increment send error count
      _senderrcnt++;

      //Ignore rest of loop
      continue;
    }

    //Increment good transaction count
    _goodxactcnt++;
  }
}
