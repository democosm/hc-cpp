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

#pragma once

#include "device.hh"
#include "thread.hh"
#include "hccell.hh"
#include "hccontainer.hh"
#include "hcmessage.hh"
#include "hcparameter.hh"
#include <fstream>
#include <inttypes.h>
#include <string>

class HCServer
{
public:
  //Maximum number of supported PIDs
  static const uint32_t PID_MAX = 65536;

  //Special reserved PIDs
  static const uint16_t PID_NAME = 0;
  static const uint16_t PID_VERSION = 1;
  static const uint16_t PID_INFOFILECRC = 2;
  static const uint16_t PID_INFOFILE = 3;

public:
  HCServer(Device *lowdev, HCContainer *top, const std::string &name, const std::string &version, uint32_t pidmax=PID_MAX);
  ~HCServer();
  HCParameter *GetParam(uint16_t pid);
  void Add(HCParameter *param);
  void Start(void);
  int GetName(std::string &val);
  int GetVersion(std::string &val);
  int GetInfoFileCRC(uint32_t &val);
  int ReadInfoFile(uint32_t offset, uint8_t *val, uint16_t maxlen, uint16_t &len);
  int GetDebug(bool &val);
  int SetDebug(bool val);
  int GetSendErrCnt(uint32_t &val);
  int GetRecvErrCnt(uint32_t &val);
  int GetDesErrCnt(uint32_t &val);
  int GetCellErrCnt(uint32_t &val);
  int GetOpCodeErrCnt(uint32_t &val);
  int GetPIDErrCnt(uint32_t &val);
  int GetIntErrCnt(uint32_t &val);
  int GetGoodXactCnt(uint32_t &val);

private:
  void SaveInfo(void);
  void SaveInfo(std::ofstream &file, uint32_t indent, HCContainer *startcont);
  bool ParamToPID(HCParameter *param, uint16_t *pid);
  void CallCmdHandler(void);
  void GetCmdHandler(void);
  void SetCmdHandler(void);
  void ICallCmdHandler(void);
  void IGetCmdHandler(void);
  void ISetCmdHandler(void);
  void AddCmdHandler(void);
  void SubCmdHandler(void);
  void ReadCmdHandler(void);
  void WriteCmdHandler(void);
  void CtlThread(void);

private:
  Device *_lowdev;
  HCContainer *_top;
  std::string _name;
  std::string _version;
  std::string _infofilename;
  uint32_t _pidtop;
  uint32_t _pidmax;
  HCParameter **_params;
  bool _started;
  HCMessage *_imsg;
  HCCell *_icell;
  HCMessage *_omsg;
  HCCell *_ocell;
  bool _debug;
  uint32_t _senderrcnt;
  uint32_t _recverrcnt;
  uint32_t _deserrcnt;
  uint32_t _cellerrcnt;
  uint32_t _opcodeerrcnt;
  uint32_t _piderrcnt;
  uint32_t _interrcnt;
  uint32_t _goodxactcnt;
  Thread<HCServer> *_ctlthread;
};
