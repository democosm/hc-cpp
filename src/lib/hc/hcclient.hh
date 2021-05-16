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

#pragma once

#include "hccell.hh"
#include "hccontainer.hh"
#include "hcmessage.hh"
#include "device.hh"
#include "event.hh"
#include "mutex.hh"
#include "thread.hh"
#include <inttypes.h>
#include <stdio.h>

class HCClient
{
public:
  HCClient(Device* lowdev, HCContainer* parent, uint32_t timeout);
  virtual ~HCClient();
  int GetDebug(bool& val);
  int SetDebug(const bool val);
  int GetSendErrCount(uint32_t& val);
  int GetRecvErrCount(uint32_t& val);
  int GetTransactionErrCount(uint32_t& val);
  int GetCellErrCount(uint32_t& val);
  int GetOpCodeErrCount(uint32_t& val);
  int GetTimeoutErrCount(uint32_t& val);
  int GetPIDErrCount(uint32_t& val);
  int GetTypeErrCount(uint32_t& val);
  int GetEIDErrCount(uint32_t& val);
  int GetOffsetErrCount(uint32_t& val);
  int GetGoodXactCount(uint32_t& val);
  int Call(uint16_t pid);
  int ICall(uint16_t pid, uint32_t eid);
  int Read(uint16_t pid, uint32_t offset, uint8_t* val, uint16_t maxlen, uint16_t& len);
  int Write(uint16_t pid, uint32_t offset, uint8_t* val, uint16_t len);
  int DownloadSIF(uint16_t pid, const char* filename);
  template <typename T> int Get(uint16_t pid, T& val);
  template <typename T> int Set(uint16_t pid, const T val);
  template <typename T> int IGet(uint16_t pid, uint32_t eid, T& val);
  template <typename T> int ISet(uint16_t pid, uint32_t eid, const T val);
  template <typename T> int Add(uint16_t pid, const T val);
  template <typename T> int Sub(uint16_t pid, const T val);
  template <typename T> int Get(uint16_t pid, T& val0, T& val1);
  template <typename T> int Set(uint16_t pid, const T val0, const T val1);
  template <typename T> int IGet(uint16_t pid, uint32_t eid, T& val0, T& val1);
  template <typename T> int ISet(uint16_t pid, uint32_t eid, const T val0, const T val1);
  template <typename T> int Get(uint16_t pid, T& val0, T& val1, T& val2);
  template <typename T> int Set(uint16_t pid, const T val0, const T val1, const T val2);
  template <typename T> int IGet(uint16_t pid, uint32_t eid, T& val0, T& val1, T& val2);
  template <typename T> int ISet(uint16_t pid, uint32_t eid, const T val0, const T val1, const T val2);
  template <typename T> int Get(uint16_t pid, T* val, uint16_t maxlen, uint16_t& len);
  template <typename T> int Set(uint16_t pid, const T* val, uint16_t len);

private:
  int CallXact(uint16_t pid);
  int GetXact(uint16_t pid, uint8_t type);
  int SetXact(uint16_t pid);
  int ICallXact(uint16_t pid, uint32_t eid);
  int IGetXact(uint16_t pid, uint32_t eid, uint8_t type);
  int ISetXact(uint16_t pid, uint32_t eid);
  int AddXact(uint16_t pid);
  int SubXact(uint16_t pid);
  int ReadXact(uint16_t pid, uint32_t offset, uint16_t maxlen);
  int WriteXact(uint16_t pid, uint32_t offset);
  void ReadThread(void);

private:
  uint32_t _pidmax;
  Device* _lowdev;
  HCContainer* _parent;
  HCMessage* _imsg;
  HCCell* _icell;
  HCMessage* _omsg;
  HCCell* _ocell;
  bool _debug;
  uint32_t _senderrcount;
  uint32_t _recverrcount;
  uint32_t _transactionerrcount;
  uint32_t _cellerrcount;
  uint32_t _opcodeerrcount;
  uint32_t _timeouterrcount;
  uint32_t _piderrcount;
  uint32_t _typeerrcount;
  uint32_t _eiderrcount;
  uint32_t _offseterrcount;
  uint32_t _goodxactcount;
  uint8_t _transaction;
  Mutex* _xactmutex;
  uint16_t _exptransaction;
  uint16_t _expopcode;
  uint32_t _timeout;
  Event* _replyevent;
  uint8_t* _filebuffer;
  Thread<HCClient>* _readthread;
};
