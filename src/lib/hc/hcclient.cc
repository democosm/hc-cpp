// HC client
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

#include "error.hh"
#include "hcclient.hh"
#include "hcboolean.hh"
#include "hcinteger.hh"
#include <cassert>
#include <iostream>

using namespace std;

static const HCBooleanEnum Offon[] =
{
  HCBooleanEnum(0, "Off"),
  HCBooleanEnum(1, "On"),
  HCBooleanEnum()
};

HCClient::HCClient(Device *lowdev, HCContainer *parent, uint32_t timeout)
{
  HCContainer *cont;

  //Assert valid arguments
  assert((lowdev != 0) && (parent != 0) && (timeout > 1000));

  //Initialize member variables
  _pidmax = 0;
  _lowdev = lowdev;
  _parent = parent;
  _imsg = new HCMessage();
  _icell = new HCCell();
  _omsg = new HCMessage();
  _ocell = new HCCell();
  _debug = false;
  _senderrcnt = 0;
  _recverrcnt = 0;
  _transactionerrcnt = 0;
  _cellerrcnt = 0;
  _opcodeerrcnt = 0;
  _timeouterrcnt = 0;
  _piderrcnt = 0;
  _typeerrcnt = 0;
  _eiderrcnt = 0;
  _offseterrcnt = 0;
  _goodxactcnt = 0;
  _transaction = 0;
  _xactmutex = new Mutex();
  _exptransaction = 0xFFFF;
  _expopcode = 0xFFFF;
  _timeout = timeout;
  _replyevent = new Event();

  //Create file buffer (only used for downloading SIF)
  _filebuffer = new uint8_t[HCCell::PAYLOAD_MAX - 9];

  //Add parameters to the parent container
  cont = new HCContainer(".client");
  parent->Add(cont);
  cont->Add(new HCBoolean<HCClient>("debug", this, &HCClient::GetDebug, &HCClient::SetDebug, Offon));
  cont->Add(new HCUns32<HCClient>("senderrcnt", this, &HCClient::GetSendErrCnt, 0));
  cont->Add(new HCUns32<HCClient>("recverrcnt", this, &HCClient::GetRecvErrCnt, 0));
  cont->Add(new HCUns32<HCClient>("transactionerrcnt", this, &HCClient::GetTransactionErrCnt, 0));
  cont->Add(new HCUns32<HCClient>("cellerrcnt", this, &HCClient::GetCellErrCnt, 0));
  cont->Add(new HCUns32<HCClient>("opcodeerrcnt", this, &HCClient::GetOpCodeErrCnt, 0));
  cont->Add(new HCUns32<HCClient>("timeouterrcnt", this, &HCClient::GetTimeoutErrCnt, 0));
  cont->Add(new HCUns32<HCClient>("piderrcnt", this, &HCClient::GetPIDErrCnt, 0));
  cont->Add(new HCUns32<HCClient>("typeerrcnt", this, &HCClient::GetTypeErrCnt, 0));
  cont->Add(new HCUns32<HCClient>("eiderrcnt", this, &HCClient::GetEIDErrCnt, 0));
  cont->Add(new HCUns32<HCClient>("offseterrcnt", this, &HCClient::GetOffsetErrCnt, 0));
  cont->Add(new HCUns32<HCClient>("goodxactcnt", this, &HCClient::GetGoodXactCnt, 0));

  //Create and start the read thread
  _readthread = new Thread<HCClient>(this, &HCClient::ReadThread);
  _readthread->Start();
}

HCClient::~HCClient()
{
  //Cleanup member variables
  delete _readthread;
  delete[] _filebuffer;
  delete _replyevent;
  delete _xactmutex;
  delete _imsg;
  delete _icell;
  delete _omsg;
  delete _ocell;
}

int HCClient::GetDebug(bool &val)
{
  //Get the value
  val = _debug;

  return ERR_NONE;
}

int HCClient::SetDebug(bool val)
{
  //Set the value
  _debug = val;

  return ERR_NONE;
}

int HCClient::GetSendErrCnt(uint32_t &val)
{
  //Get the value
  val = _senderrcnt;

  return ERR_NONE;
}

int HCClient::GetRecvErrCnt(uint32_t &val)
{
  //Get the value
  val = _recverrcnt;

  return ERR_NONE;
}

int HCClient::GetTransactionErrCnt(uint32_t &val)
{
  //Get the value
  val = _transactionerrcnt;

  return ERR_NONE;
}

int HCClient::GetCellErrCnt(uint32_t &val)
{
  //Get the value
  val = _cellerrcnt;

  return ERR_NONE;
}

int HCClient::GetOpCodeErrCnt(uint32_t &val)
{
  //Get the value
  val = _opcodeerrcnt;

  return ERR_NONE;
}

int HCClient::GetTimeoutErrCnt(uint32_t &val)
{
  //Get the value
  val = _timeouterrcnt;

  return ERR_NONE;
}

int HCClient::GetPIDErrCnt(uint32_t &val)
{
  //Get the value
  val = _piderrcnt;

  return ERR_NONE;
}

int HCClient::GetTypeErrCnt(uint32_t &val)
{
  //Get the value
  val = _typeerrcnt;

  return ERR_NONE;
}

int HCClient::GetEIDErrCnt(uint32_t &val)
{
  //Get the value
  val = _eiderrcnt;

  return ERR_NONE;
}

int HCClient::GetOffsetErrCnt(uint32_t &val)
{
  //Get the value
  val = _offseterrcnt;

  return ERR_NONE;
}

int HCClient::GetGoodXactCnt(uint32_t &val)
{
  //Get the value
  val = _goodxactcnt;

  return ERR_NONE;
}

int HCClient::Call(uint16_t pid)
{
  int ierr;

  //Begin mutual exclusion of transaction
  _xactmutex->Wait();

  //Format outbound message
  _omsg->Reset(_transaction);
  _ocell->Reset(HCCell::OPCODE_CALL_CMD);
  _ocell->Write(pid);
  _omsg->Write(_ocell);

  //Perform call transaction
  ierr = CallXact(pid);

  //End mutual exclusion of transaction
  _xactmutex->Give();

  return ierr;
}

int HCClient::ICall(uint16_t pid, uint32_t eid)
{
  int ierr;

  //Begin mutual exclusion of transaction
  _xactmutex->Wait();

  //Format outbound message
  _omsg->Reset(_transaction);
  _ocell->Reset(HCCell::OPCODE_ICALL_CMD);
  _ocell->Write(pid);
  _ocell->Write(eid);
  _omsg->Write(_ocell);

  //Perform set transaction
  ierr = ICallXact(pid, eid);

  //End mutual exclusion of transaction
  _xactmutex->Give();

  return ierr;
}

int HCClient::Read(uint16_t pid, uint32_t offset, uint8_t *val, uint16_t maxlen, uint16_t &len)
{
  int8_t merr;
  int ierr;

  //Take the transaction mutex
  _xactmutex->Wait();

  //Perform get transaction
  ierr = ReadXact(pid, offset, maxlen);

  //Check for no internal error
  if(ierr == ERR_NONE)
  {
    //Read value and error from inbound cell (already skipped past PID, and offset)
    _icell->Read(val, maxlen, len);
    _icell->Read(merr);
    ierr = (int)merr;
  }
  else
  {
    //Indicate no bytes read
    len = 0;
  }

  //Give the transaction mutex
  _xactmutex->Give();

  return ierr;
}

int HCClient::Write(uint16_t pid, uint32_t offset, uint8_t *val, uint16_t len)
{
  int ierr;

  //Begin mutual exclusion of transaction
  _xactmutex->Wait();

  //Format outbound message
  _omsg->Reset(_transaction);
  _ocell->Reset(HCCell::OPCODE_WRITE_CMD);
  _ocell->Write(pid);
  _ocell->Write(offset);
  _ocell->Write(val, len);
  _omsg->Write(_ocell);

  //Perform write transaction
  ierr = WriteXact(pid, offset);

  //End mutual exclusion of transaction
  _xactmutex->Give();

  return ierr;
}

int HCClient::DownloadSIF(uint16_t pid, const char *filename)
{
  FILE *file;
  uint16_t len;
  int ierr;

  //Assert valid arguments
  assert(filename != 0);

  //Open local file and check for error
  if((file = fopen(filename, "w")) == NULL)
    return ERR_UNSPEC;

  //Transfer file piece by piece
  while(true)
  {
    //Read part of remote file
    ierr = Read(pid, (uint32_t)ftell(file), _filebuffer, HCCell::PAYLOAD_MAX - 9, len);

    //If timeout, retry read part of remote file
    if(ierr == ERR_TIMEOUT)
      ierr = Read(pid, (uint32_t)ftell(file), _filebuffer, HCCell::PAYLOAD_MAX - 9, len);

    //If timeout, retry read part of remote file
    if(ierr == ERR_TIMEOUT)
      ierr = Read(pid, (uint32_t)ftell(file), _filebuffer, HCCell::PAYLOAD_MAX - 9, len);

    //Check for error
    if(ierr != ERR_NONE)
    {
      //Close info file
      fclose(file);
      return ierr;
    }

    //Write to local file
    fwrite(_filebuffer, 1, len, file);

    //Check for done
    if(len < HCCell::PAYLOAD_MAX - 9)
      break;
  }

  //Close info file
  fclose(file);
  return ERR_NONE;
}

template <typename T> int HCClient::Get(uint16_t pid, T &val)
{
  uint8_t type;
  int8_t merr;
  int ierr;

  //Determine type code
  type = HCParameter::TypeCode(val);

  //Take the transaction mutex
  _xactmutex->Wait();

  //Perform get transaction
  ierr = GetXact(pid, type);

  //Check for no internal error
  if(ierr == ERR_NONE)
  {
    //Read value and error from inbound cell (already skipped past PID and type)
    _icell->Read(val);
    _icell->Read(merr);
    ierr = (int)merr;
  }
  else
  {
    //Set to default value
    HCParameter::DefaultVal(val);
  }

  //Give the transaction mutex
  _xactmutex->Give();

  return ierr;
}

template <typename T> int HCClient::Set(uint16_t pid, const T val)
{
  uint8_t type;
  int ierr;

  //Determine type code
  type = HCParameter::TypeCode(val);

  //Begin mutual exclusion of transaction
  _xactmutex->Wait();

  //Format outbound message
  _omsg->Reset(_transaction);
  _ocell->Reset(HCCell::OPCODE_SET_CMD);
  _ocell->Write(pid);
  _ocell->Write(type);
  _ocell->Write(val);
  _omsg->Write(_ocell);

  //Perform set transaction
  ierr = SetXact(pid);

  //End mutual exclusion of transaction
  _xactmutex->Give();

  return ierr;
}

template <typename T> int HCClient::IGet(uint16_t pid, uint32_t eid, T &val)
{
  uint8_t type;
  int8_t merr;
  int ierr;

  //Determine type code
  type = HCParameter::TypeCode(val);

  //Take the transaction mutex
  _xactmutex->Wait();

  //Perform get transaction
  ierr = IGetXact(pid, eid, type);

  //Check for no internal error
  if(ierr == ERR_NONE)
  {
    //Read value and error from inbound cell (already skipped past PID and EID)
    _icell->Read(val);
    _icell->Read(merr);
    ierr = (int)merr;
  }
  else
  {
    //Set to default value
    HCParameter::DefaultVal(val);
  }

  //Give the transaction mutex
  _xactmutex->Give();

  return ierr;
}

template <typename T> int HCClient::ISet(uint16_t pid, uint32_t eid, const T val)
{
  uint8_t type;
  int ierr;

  //Determine type code
  type = HCParameter::TypeCode(val);

  //Begin mutual exclusion of transaction
  _xactmutex->Wait();

  //Format outbound message
  _omsg->Reset(_transaction);
  _ocell->Reset(HCCell::OPCODE_ISET_CMD);
  _ocell->Write(pid);
  _ocell->Write(eid);
  _ocell->Write(type);
  _ocell->Write(val);
  _omsg->Write(_ocell);

  //Perform set transaction
  ierr = ISetXact(pid, eid);

  //End mutual exclusion of transaction
  _xactmutex->Give();

  return ierr;
}

template <typename T> int HCClient::Add(uint16_t pid, const T val)
{
  uint8_t type;
  int ierr;

  //Determine type code
  type = HCParameter::TypeCode(val);

  //Begin mutual exclusion of transaction
  _xactmutex->Wait();

  //Format outbound message
  _omsg->Reset(_transaction);
  _ocell->Reset(HCCell::OPCODE_ADD_CMD);
  _ocell->Write(pid);
  _ocell->Write(type);
  _ocell->Write(val);
  _omsg->Write(_ocell);

  //Perform add transaction
  ierr = AddXact(pid);

  //End mutual exclusion of transaction
  _xactmutex->Give();

  return ierr;
}

template <typename T> int HCClient::Sub(uint16_t pid, const T val)
{
  uint8_t type;
  int ierr;

  //Determine type code
  type = HCParameter::TypeCode(val);

  //Begin mutual exclusion of transaction
  _xactmutex->Wait();

  //Format outbound message
  _omsg->Reset(_transaction);
  _ocell->Reset(HCCell::OPCODE_SUB_CMD);
  _ocell->Write(pid);
  _ocell->Write(type);
  _ocell->Write(val);
  _omsg->Write(_ocell);

  //Perform sub transaction
  ierr = SubXact(pid);

  //End mutual exclusion of transaction
  _xactmutex->Give();

  return ierr;
}

template int HCClient::Get<int8_t>(uint16_t pid, int8_t &val);
template int HCClient::Set<int8_t>(uint16_t pid, const int8_t val);
template int HCClient::IGet<int8_t>(uint16_t pid, uint32_t eid, int8_t &val);
template int HCClient::ISet<int8_t>(uint16_t pid, uint32_t eid, const int8_t val);
template int HCClient::Add<int8_t>(uint16_t pid, const int8_t val);
template int HCClient::Sub<int8_t>(uint16_t pid, const int8_t val);

template int HCClient::Get<int16_t>(uint16_t pid, int16_t &val);
template int HCClient::Set<int16_t>(uint16_t pid, const int16_t val);
template int HCClient::IGet<int16_t>(uint16_t pid, uint32_t eid, int16_t &val);
template int HCClient::ISet<int16_t>(uint16_t pid, uint32_t eid, const int16_t val);
template int HCClient::Add<int16_t>(uint16_t pid, const int16_t val);
template int HCClient::Sub<int16_t>(uint16_t pid, const int16_t val);

template int HCClient::Get<int32_t>(uint16_t pid, int32_t &val);
template int HCClient::Set<int32_t>(uint16_t pid, const int32_t val);
template int HCClient::IGet<int32_t>(uint16_t pid, uint32_t eid, int32_t &val);
template int HCClient::ISet<int32_t>(uint16_t pid, uint32_t eid, const int32_t val);
template int HCClient::Add<int32_t>(uint16_t pid, const int32_t val);
template int HCClient::Sub<int32_t>(uint16_t pid, const int32_t val);

template int HCClient::Get<int64_t>(uint16_t pid, int64_t &val);
template int HCClient::Set<int64_t>(uint16_t pid, const int64_t val);
template int HCClient::IGet<int64_t>(uint16_t pid, uint32_t eid, int64_t &val);
template int HCClient::ISet<int64_t>(uint16_t pid, uint32_t eid, const int64_t val);
template int HCClient::Add<int64_t>(uint16_t pid, const int64_t val);
template int HCClient::Sub<int64_t>(uint16_t pid, const int64_t val);

template int HCClient::Get<uint8_t>(uint16_t pid, uint8_t &val);
template int HCClient::Set<uint8_t>(uint16_t pid, const uint8_t val);
template int HCClient::IGet<uint8_t>(uint16_t pid, uint32_t eid, uint8_t &val);
template int HCClient::ISet<uint8_t>(uint16_t pid, uint32_t eid, const uint8_t val);
template int HCClient::Add<uint8_t>(uint16_t pid, const uint8_t val);
template int HCClient::Sub<uint8_t>(uint16_t pid, const uint8_t val);

template int HCClient::Get<uint16_t>(uint16_t pid, uint16_t &val);
template int HCClient::Set<uint16_t>(uint16_t pid, const uint16_t val);
template int HCClient::IGet<uint16_t>(uint16_t pid, uint32_t eid, uint16_t &val);
template int HCClient::ISet<uint16_t>(uint16_t pid, uint32_t eid, const uint16_t val);
template int HCClient::Add<uint16_t>(uint16_t pid, const uint16_t val);
template int HCClient::Sub<uint16_t>(uint16_t pid, const uint16_t val);

template int HCClient::Get<uint32_t>(uint16_t pid, uint32_t &val);
template int HCClient::Set<uint32_t>(uint16_t pid, const uint32_t val);
template int HCClient::IGet<uint32_t>(uint16_t pid, uint32_t eid, uint32_t &val);
template int HCClient::ISet<uint32_t>(uint16_t pid, uint32_t eid, const uint32_t val);
template int HCClient::Add<uint32_t>(uint16_t pid, const uint32_t val);
template int HCClient::Sub<uint32_t>(uint16_t pid, const uint32_t val);

template int HCClient::Get<uint64_t>(uint16_t pid, uint64_t &val);
template int HCClient::Set<uint64_t>(uint16_t pid, const uint64_t val);
template int HCClient::IGet<uint64_t>(uint16_t pid, uint32_t eid, uint64_t &val);
template int HCClient::ISet<uint64_t>(uint16_t pid, uint32_t eid, const uint64_t val);
template int HCClient::Add<uint64_t>(uint16_t pid, const uint64_t val);
template int HCClient::Sub<uint64_t>(uint16_t pid, const uint64_t val);

template int HCClient::Get<float>(uint16_t pid, float &val);
template int HCClient::Set<float>(uint16_t pid, const float val);
template int HCClient::IGet<float>(uint16_t pid, uint32_t eid, float &val);
template int HCClient::ISet<float>(uint16_t pid, uint32_t eid, const float val);
template int HCClient::Add<float>(uint16_t pid, const float val);
template int HCClient::Sub<float>(uint16_t pid, const float val);

template int HCClient::Get<double>(uint16_t pid, double &val);
template int HCClient::Set<double>(uint16_t pid, const double val);
template int HCClient::IGet<double>(uint16_t pid, uint32_t eid, double &val);
template int HCClient::ISet<double>(uint16_t pid, uint32_t eid, const double val);
template int HCClient::Add<double>(uint16_t pid, const double val);
template int HCClient::Sub<double>(uint16_t pid, const double val);

template int HCClient::Get<bool>(uint16_t pid, bool &val);
template int HCClient::Set<bool>(uint16_t pid, const bool val);
template int HCClient::IGet<bool>(uint16_t pid, uint32_t eid, bool &val);
template int HCClient::ISet<bool>(uint16_t pid, uint32_t eid, const bool val);
template int HCClient::Add<bool>(uint16_t pid, const bool val);
template int HCClient::Sub<bool>(uint16_t pid, const bool val);

template int HCClient::Get<string>(uint16_t pid, string &val);
template int HCClient::Set<string>(uint16_t pid, const string val);
template int HCClient::IGet<string>(uint16_t pid, uint32_t eid, string &val);
template int HCClient::ISet<string>(uint16_t pid, uint32_t eid, const string val);
template int HCClient::Add<string>(uint16_t pid, const string val);
template int HCClient::Sub<string>(uint16_t pid, const string val);

template <typename T> int HCClient::Get(uint16_t pid, T &val0, T &val1, T &val2)
{
  uint8_t type;
  int8_t merr;
  int ierr;

  //Determine type code
  type = HCParameter::TypeCode(val0, val1, val2);

  //Take the transaction mutex
  _xactmutex->Wait();

  //Perform get transaction
  ierr = GetXact(pid, type);

  //Check for no internal error
  if(ierr == ERR_NONE)
  {
    //Read values and error from inbound cell (already skipped past PID and type)
    _icell->Read(val0);
    _icell->Read(val1);
    _icell->Read(val2);
    _icell->Read(merr);
    ierr = (int)merr;
  }
  else
  {
    //Set to default value
    HCParameter::DefaultVal(val0, val1, val2);
  }

  //Give the transaction mutex
  _xactmutex->Give();

  return ierr;
}

template <typename T> int HCClient::Set(uint16_t pid, const T val0, const T val1, const T val2)
{
  uint8_t type;
  int ierr;

  //Determine type code
  type = HCParameter::TypeCode(val0, val1, val2);

  //Begin mutual exclusion of transaction
  _xactmutex->Wait();

  //Format outbound message
  _omsg->Reset(_transaction);
  _ocell->Reset(HCCell::OPCODE_SET_CMD);
  _ocell->Write(pid);
  _ocell->Write(type);
  _ocell->Write(val0);
  _ocell->Write(val1);
  _ocell->Write(val2);
  _omsg->Write(_ocell);

  //Perform set transaction
  ierr = SetXact(pid);

  //End mutual exclusion of transaction
  _xactmutex->Give();

  return ierr;
}

template <typename T> int HCClient::IGet(uint16_t pid, uint32_t eid, T &val0, T &val1, T &val2)
{
  uint8_t type;
  int8_t merr;
  int ierr;

  //Determine type code
  type = HCParameter::TypeCode(val0, val1, val2);

  //Take the transaction mutex
  _xactmutex->Wait();

  //Perform get transaction
  ierr = IGetXact(pid, eid, type);

  //Check for no internal error
  if(ierr == ERR_NONE)
  {
    //Read values and error from inbound cell (already skipped past PID and EID)
    _icell->Read(val0);
    _icell->Read(val1);
    _icell->Read(val2);
    _icell->Read(merr);
    ierr = (int)merr;
  }
  else
  {
    //Set to default value
    HCParameter::DefaultVal(val0, val1, val2);
  }

  //Give the transaction mutex
  _xactmutex->Give();

  return ierr;
}

template <typename T> int HCClient::ISet(uint16_t pid, uint32_t eid, const T val0, const T val1, const T val2)
{
  uint8_t type;
  int ierr;

  //Determine type code
  type = HCParameter::TypeCode(val0, val1, val2);

  //Begin mutual exclusion of transaction
  _xactmutex->Wait();

  //Format outbound message
  _omsg->Reset(_transaction);
  _ocell->Reset(HCCell::OPCODE_ISET_CMD);
  _ocell->Write(pid);
  _ocell->Write(eid);
  _ocell->Write(type);
  _ocell->Write(val0);
  _ocell->Write(val1);
  _ocell->Write(val2);
  _omsg->Write(_ocell);

  //Perform set transaction
  ierr = ISetXact(pid, eid);

  //End mutual exclusion of transaction
  _xactmutex->Give();

  return ierr;
}

template int HCClient::Get<float>(uint16_t pid, float &val0, float &val1, float &val2);
template int HCClient::Set<float>(uint16_t pid, const float val0, const float val1, const float val2);
template int HCClient::IGet<float>(uint16_t pid, uint32_t eid, float &val0, float &val1, float &val2);
template int HCClient::ISet<float>(uint16_t pid, uint32_t eid, const float val0, const float val1, const float val2);

template int HCClient::Get<double>(uint16_t pid, double &val0, double &val1, double &val2);
template int HCClient::Set<double>(uint16_t pid, const double val0, const double val1, const double val2);
template int HCClient::IGet<double>(uint16_t pid, uint32_t eid, double &val0, double &val1, double &val2);
template int HCClient::ISet<double>(uint16_t pid, uint32_t eid, const double val0, const double val1, const double val2);

int HCClient::CallXact(uint16_t pid)
{
  uint16_t ipid;
  int8_t berr;

  //Reset reply event
  _replyevent->Reset();

  //Set expected reply parameters
  _exptransaction = _transaction;
  _expopcode = HCCell::OPCODE_CALL_STS;

  //Increment transaction number
  _transaction++;

  //Print outbound message if requested
  if(_debug)
    _omsg->Print("Tx");

  //Send outbound message and check for error
  if(_omsg->Send(_lowdev) != ERR_NONE)
  {
    //Increment send error count
    _senderrcnt++;

    //Set expected reply parameters to invalid
    _exptransaction = 0xFFFF;
    _expopcode = 0xFFFF;

    return ERR_UNSPEC;
  }

  //Wait for response
  if(_replyevent->Wait(_timeout) != 0)
  {
    //Increment timeout error count
    _timeouterrcnt++;

    //Set expected reply parameters to invalid
    _exptransaction = 0xFFFF;
    _expopcode = 0xFFFF;

    return ERR_TIMEOUT;
  }

  //Set expected reply parameters to invalid
  _exptransaction = 0xFFFF;
  _expopcode = 0xFFFF;

  //Read PID from inbound cell
  _icell->Read(ipid);

  //Check for inbound PID doesn't match outbound PID
  if(ipid != pid)
  {
    //Increment PID error count
    _piderrcnt++;

    return ERR_UNSPEC;
  }

  //Read error code from inbound cell
  _icell->Read(berr);

  //Increment good transaction count
  _goodxactcnt++;

  return (int)berr;
}

int HCClient::GetXact(uint16_t pid, uint8_t type)
{
  uint16_t ipid;
  uint8_t itype;

  //Reset reply event
  _replyevent->Reset();

  //Set expected reply parameters
  _exptransaction = _transaction;
  _expopcode = HCCell::OPCODE_GET_STS;

  //Format outbound message
  _omsg->Reset(_transaction++);
  _ocell->Reset(HCCell::OPCODE_GET_CMD);
  _ocell->Write(pid);
  _omsg->Write(_ocell);

  //Print outbound message if requested
  if(_debug)
    _omsg->Print("Tx");

  //Send outbound message and check for error
  if(_omsg->Send(_lowdev) != ERR_NONE)
  {
    //Increment send error count
    _senderrcnt++;

    //Set expected reply parameters to invalid
    _exptransaction = 0xFFFF;
    _expopcode = 0xFFFF;

    return ERR_UNSPEC;
  }

  //Wait for response
  if(_replyevent->Wait(_timeout) != 0)
  {
    //Increment timeout error count
    _timeouterrcnt++;

    //Set expected reply parameters to invalid
    _exptransaction = 0xFFFF;
    _expopcode = 0xFFFF;

    return ERR_TIMEOUT;
  }

  //Set expected reply parameters to invalid
  _exptransaction = 0xFFFF;
  _expopcode = 0xFFFF;

  //Read PID from inbound cell
  _icell->Read(ipid);

  //Check for inbound PID doesn't match outbound PID
  if(ipid != pid)
  {
    //Increment PID error count
    _piderrcnt++;

    return ERR_UNSPEC;
  }

  //Read parameter type from inbound cell
  _icell->Read(itype);

  //Check for inbound type doesn't match expected type
  if(itype != type)
  {
    //Increment type error count
    _typeerrcnt++;

    return ERR_TYPE;
  }

  //Increment good transaction count
  _goodxactcnt++;

  return ERR_NONE;
}

int HCClient::SetXact(uint16_t pid)
{
  uint16_t ipid;
  int8_t berr;

  //Reset reply event
  _replyevent->Reset();

  //Set expected reply parameters
  _exptransaction = _transaction;
  _expopcode = HCCell::OPCODE_SET_STS;

  //Increment transaction number
  _transaction++;

  //Print outbound message if requested
  if(_debug)
    _omsg->Print("Tx");

  //Send outbound message and check for error
  if(_omsg->Send(_lowdev) != ERR_NONE)
  {
    //Increment send error count
    _senderrcnt++;

    //Set expected reply parameters to invalid
    _exptransaction = 0xFFFF;
    _expopcode = 0xFFFF;

    return ERR_UNSPEC;
  }

  //Wait for response
  if(_replyevent->Wait(_timeout) != 0)
  {
    //Increment timeout error count
    _timeouterrcnt++;

    //Set expected reply parameters to invalid
    _exptransaction = 0xFFFF;
    _expopcode = 0xFFFF;

    return ERR_TIMEOUT;
  }

  //Set expected reply parameters to invalid
  _exptransaction = 0xFFFF;
  _expopcode = 0xFFFF;

  //Read PID from inbound cell
  _icell->Read(ipid);

  //Check for inbound PID doesn't match outbound PID
  if(ipid != pid)
  {
    //Increment PID error count
    _piderrcnt++;

    return ERR_UNSPEC;
  }

  //Read error code from inbound cell
  _icell->Read(berr);

  //Increment good transaction count
  _goodxactcnt++;

  return (int)berr;
}

int HCClient::ICallXact(uint16_t pid, uint32_t eid)
{
  uint16_t ipid;
  uint32_t ieid;
  int8_t berr;

  //Reset reply event
  _replyevent->Reset();

  //Set expected reply parameters
  _exptransaction = _transaction;
  _expopcode = HCCell::OPCODE_ICALL_STS;

  //Increment transaction number
  _transaction++;

  //Print outbound message if requested
  if(_debug)
    _omsg->Print("Tx");

  //Send outbound message and check for error
  if(_omsg->Send(_lowdev) != ERR_NONE)
  {
    //Increment send error count
    _senderrcnt++;

    //Set expected reply parameters to invalid
    _exptransaction = 0xFFFF;
    _expopcode = 0xFFFF;

    return ERR_UNSPEC;
  }

  //Wait for response
  if(_replyevent->Wait(_timeout) != 0)
  {
    //Increment timeout error count
    _timeouterrcnt++;

    //Set expected reply parameters to invalid
    _exptransaction = 0xFFFF;
    _expopcode = 0xFFFF;

    return ERR_TIMEOUT;
  }

  //Set expected reply parameters to invalid
  _exptransaction = 0xFFFF;
  _expopcode = 0xFFFF;

  //Read PID from inbound cell
  _icell->Read(ipid);

  //Check for inbound PID doesn't match outbound PID
  if(ipid != pid)
  {
    //Increment PID error count
    _piderrcnt++;

    return ERR_UNSPEC;
  }

  //Read EID from inbound cell
  _icell->Read(ieid);

  //Check for inbound EID doesn't match outbound EID
  if(ieid != eid)
  {
    //Increment EID error count
    _eiderrcnt++;

    return ERR_UNSPEC;
  }

  //Read error code from inbound cell
  _icell->Read(berr);

  //Increment good transaction count
  _goodxactcnt++;

  return (int)berr;
}

int HCClient::IGetXact(uint16_t pid, uint32_t eid, uint8_t type)
{
  uint16_t ipid;
  uint32_t ieid;
  uint8_t itype;

  //Reset reply event
  _replyevent->Reset();

  //Set expected reply parameters
  _exptransaction = _transaction;
  _expopcode = HCCell::OPCODE_IGET_STS;

  //Format outbound message
  _omsg->Reset(_transaction++);
  _ocell->Reset(HCCell::OPCODE_IGET_CMD);
  _ocell->Write(pid);
  _ocell->Write(eid);
  _omsg->Write(_ocell);

  //Print outbound message if requested
  if(_debug)
    _omsg->Print("Tx");

  //Send outbound message and check for error
  if(_omsg->Send(_lowdev) != ERR_NONE)
  {
    //Increment send error count
    _senderrcnt++;

    //Set expected reply parameters to invalid
    _exptransaction = 0xFFFF;
    _expopcode = 0xFFFF;

    return ERR_UNSPEC;
  }

  //Wait for response
  if(_replyevent->Wait(_timeout) != 0)
  {
    //Increment timeout error count
    _timeouterrcnt++;

    //Set expected reply parameters to invalid
    _exptransaction = 0xFFFF;
    _expopcode = 0xFFFF;

    return ERR_TIMEOUT;
  }

  //Set expected reply parameters to invalid
  _exptransaction = 0xFFFF;
  _expopcode = 0xFFFF;

  //Read PID from inbound cell
  _icell->Read(ipid);

  //Check for inbound PID doesn't match outbound PID
  if(ipid != pid)
  {
    //Increment PID error count
    _piderrcnt++;

    return ERR_UNSPEC;
  }

  //Read EID from inbound cell
  _icell->Read(ieid);

  //Check for inbound EID doesn't match outbound EID
  if(ieid != eid)
  {
    //Increment EID error count
    _eiderrcnt++;

    return ERR_UNSPEC;
  }

  //Read parameter type from inbound cell
  _icell->Read(itype);

  //Check for inbound type doesn't match expected type
  if(itype != type)
  {
    //Increment type error count
    _typeerrcnt++;

    return ERR_TYPE;
  }

  //Increment good transaction count
  _goodxactcnt++;

  return ERR_NONE;
}

int HCClient::ISetXact(uint16_t pid, uint32_t eid)
{
  uint16_t ipid;
  uint32_t ieid;
  int8_t berr;

  //Reset reply event
  _replyevent->Reset();

  //Set expected reply parameters
  _exptransaction = _transaction;
  _expopcode = HCCell::OPCODE_ISET_STS;

  //Increment transaction number
  _transaction++;

  //Print outbound message if requested
  if(_debug)
    _omsg->Print("Tx");

  //Send outbound message and check for error
  if(_omsg->Send(_lowdev) != ERR_NONE)
  {
    //Increment send error count
    _senderrcnt++;

    //Set expected reply parameters to invalid
    _exptransaction = 0xFFFF;
    _expopcode = 0xFFFF;

    return ERR_UNSPEC;
  }

  //Wait for response
  if(_replyevent->Wait(_timeout) != 0)
  {
    //Increment timeout error count
    _timeouterrcnt++;

    //Set expected reply parameters to invalid
    _exptransaction = 0xFFFF;
    _expopcode = 0xFFFF;

    return ERR_TIMEOUT;
  }

  //Set expected reply parameters to invalid
  _exptransaction = 0xFFFF;
  _expopcode = 0xFFFF;

  //Read PID from inbound cell
  _icell->Read(ipid);

  //Check for inbound PID doesn't match outbound PID
  if(ipid != pid)
  {
    //Increment PID error count
    _piderrcnt++;

    return ERR_UNSPEC;
  }

  //Read EID from inbound cell
  _icell->Read(ieid);

  //Check for inbound EID doesn't match outbound EID
  if(ieid != eid)
  {
    //Increment EID error count
    _eiderrcnt++;

    return ERR_UNSPEC;
  }

  //Read error code from inbound cell
  _icell->Read(berr);

  //Increment good transaction count
  _goodxactcnt++;

  return (int)berr;
}

int HCClient::AddXact(uint16_t pid)
{
  uint16_t ipid;
  int8_t berr;

  //Reset reply event
  _replyevent->Reset();

  //Set expected reply parameters
  _exptransaction = _transaction;
  _expopcode = HCCell::OPCODE_ADD_STS;

  //Increment transaction number
  _transaction++;

  //Print outbound message if requested
  if(_debug)
    _omsg->Print("Tx");

  //Send outbound message and check for error
  if(_omsg->Send(_lowdev) != ERR_NONE)
  {
    //Increment send error count
    _senderrcnt++;

    //Set expected reply parameters to invalid
    _exptransaction = 0xFFFF;
    _expopcode = 0xFFFF;

    return ERR_UNSPEC;
  }

  //Wait for response
  if(_replyevent->Wait(_timeout) != 0)
  {
    //Increment timeout error count
    _timeouterrcnt++;

    //Set expected reply parameters to invalid
    _exptransaction = 0xFFFF;
    _expopcode = 0xFFFF;

    return ERR_TIMEOUT;
  }

  //Set expected reply parameters to invalid
  _exptransaction = 0xFFFF;
  _expopcode = 0xFFFF;

  //Read PID from inbound cell
  _icell->Read(ipid);

  //Check for inbound PID doesn't match outbound PID
  if(ipid != pid)
  {
    //Increment PID error count
    _piderrcnt++;

    return ERR_UNSPEC;
  }

  //Read error code from inbound cell
  _icell->Read(berr);

  //Increment good transaction count
  _goodxactcnt++;

  return (int)berr;
}

int HCClient::SubXact(uint16_t pid)
{
  uint16_t ipid;
  int8_t berr;

  //Reset reply event
  _replyevent->Reset();

  //Set expected reply parameters
  _exptransaction = _transaction;
  _expopcode = HCCell::OPCODE_SUB_STS;

  //Increment transaction number
  _transaction++;

  //Print outbound message if requested
  if(_debug)
    _omsg->Print("Tx");

  //Send outbound message and check for error
  if(_omsg->Send(_lowdev) != ERR_NONE)
  {
    //Increment send error count
    _senderrcnt++;

    //Set expected reply parameters to invalid
    _exptransaction = 0xFFFF;
    _expopcode = 0xFFFF;

    return ERR_UNSPEC;
  }

  //Wait for response
  if(_replyevent->Wait(_timeout) != 0)
  {
    //Increment timeout error count
    _timeouterrcnt++;

    //Set expected reply parameters to invalid
    _exptransaction = 0xFFFF;
    _expopcode = 0xFFFF;

    return ERR_TIMEOUT;
  }

  //Set expected reply parameters to invalid
  _exptransaction = 0xFFFF;
  _expopcode = 0xFFFF;

  //Read PID from inbound cell
  _icell->Read(ipid);

  //Check for inbound PID doesn't match outbound PID
  if(ipid != pid)
  {
    //Increment PID error count
    _piderrcnt++;

    return ERR_UNSPEC;
  }

  //Read error code from inbound cell
  _icell->Read(berr);

  //Increment good transaction count
  _goodxactcnt++;

  return (int)berr;
}

int HCClient::ReadXact(uint16_t pid, uint32_t offset, uint16_t maxlen)
{
  uint16_t ipid;
  uint32_t ioffset;

  //Reset reply event
  _replyevent->Reset();

  //Set expected reply parameters
  _exptransaction = _transaction;
  _expopcode = HCCell::OPCODE_READ_STS;

  //Format outbound message
  _omsg->Reset(_transaction++);
  _ocell->Reset(HCCell::OPCODE_READ_CMD);
  _ocell->Write(pid);
  _ocell->Write(offset);
  _ocell->Write(maxlen);
  _omsg->Write(_ocell);

  //Print outbound message if requested
  if(_debug)
    _omsg->Print("Tx");

  //Send outbound message and check for error
  if(_omsg->Send(_lowdev) != ERR_NONE)
  {
    //Increment send error count
    _senderrcnt++;

    //Set expected reply parameters to invalid
    _exptransaction = 0xFFFF;
    _expopcode = 0xFFFF;

    return ERR_UNSPEC;
  }

  //Wait for response
  if(_replyevent->Wait(_timeout) != 0)
  {
    //Increment timeout error count
    _timeouterrcnt++;

    //Set expected reply parameters to invalid
    _exptransaction = 0xFFFF;
    _expopcode = 0xFFFF;

    return ERR_TIMEOUT;
  }

  //Set expected reply parameters to invalid
  _exptransaction = 0xFFFF;
  _expopcode = 0xFFFF;

  //Read PID from inbound cell
  _icell->Read(ipid);

  //Check for inbound PID doesn't match outbound PID
  if(ipid != pid)
  {
    //Increment PID error count
    _piderrcnt++;

    return ERR_UNSPEC;
  }

  //Read offset from inbound cell
  _icell->Read(ioffset);

  //Check for inbound offset doesn't match outbound offset
  if(ioffset != offset)
  {
    //Increment offset error count
    _offseterrcnt++;

    return ERR_UNSPEC;
  }

  //Increment good transaction count
  _goodxactcnt++;

  return ERR_NONE;
}

int HCClient::WriteXact(uint16_t pid, uint32_t offset)
{
  uint16_t ipid;
  uint32_t ioffset;
  int8_t berr;

  //Reset reply event
  _replyevent->Reset();

  //Set expected reply parameters
  _exptransaction = _transaction;
  _expopcode = HCCell::OPCODE_WRITE_STS;

  //Increment transaction number
  _transaction++;

  //Print outbound message if requested
  if(_debug)
    _omsg->Print("Tx");

  //Send outbound message and check for error
  if(_omsg->Send(_lowdev) != ERR_NONE)
  {
    //Increment send error count
    _senderrcnt++;

    //Set expected reply parameters to invalid
    _exptransaction = 0xFFFF;
    _expopcode = 0xFFFF;

    return ERR_UNSPEC;
  }

  //Wait for response
  if(_replyevent->Wait(_timeout) != 0)
  {
    //Increment timeout error count
    _timeouterrcnt++;

    //Set expected reply parameters to invalid
    _exptransaction = 0xFFFF;
    _expopcode = 0xFFFF;

    return ERR_TIMEOUT;
  }

  //Set expected reply parameters to invalid
  _exptransaction = 0xFFFF;
  _expopcode = 0xFFFF;

  //Read PID from inbound cell
  _icell->Read(ipid);

  //Check for inbound PID doesn't match outbound PID
  if(ipid != pid)
  {
    //Increment PID error count
    _piderrcnt++;

    return ERR_UNSPEC;
  }

  //Read offset from inbound cell
  _icell->Read(ioffset);

  //Check for inbound offset doesn't match outbound offset
  if(ioffset != offset)
  {
    //Increment offset error count
    _offseterrcnt++;

    return ERR_UNSPEC;
  }

  //Read error code from inbound cell
  _icell->Read(berr);

  //Increment good transaction count
  _goodxactcnt++;

  return (int)berr;
}

void HCClient::ReadThread(void)
{
  //Go forever
  while(true)
  {
    //Receive inbound message and check for error
    if(_imsg->Recv(_lowdev) != ERR_NONE)
    {
      //Increment receive error count
      _recverrcnt++;

      //Ignore the rest of this loop
      continue;
    }

    //Print inbound message if requested
    if(_debug)
      _imsg->Print("Rx");

    //Check for invalid transaction number
    if(_imsg->GetTransaction() != _exptransaction)
    {
      //Increment transaction error count
      _transactionerrcnt++;

      //Ignore the rest of this loop
      continue;
    }

    //Read inbound cell from message and check for error
    if(!_imsg->Read(_icell))
    {
      //Increment cell error count
      _cellerrcnt++;

      //Ignore the rest of this loop
      continue;
    }

    //Check for invalid opcode
    if(_icell->GetOpCode() != _expopcode)
    {
      //Increment opcode error count
      _opcodeerrcnt++;

      //Ignore the rest of this loop
      continue;
    }

    //Signal valid reply
    _replyevent->Signal();
  }
}
