// HC query server
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

#ifndef _HCQSERVER_HH_
#define _HCQSERVER_HH_

#include "device.hh"
#include "hccontainer.hh"
#include "thread.hh"

class HCQServer
{
public:
  HCQServer(Device *lowdev, HCContainer *top);
  ~HCQServer();

private:
  bool NextReadCharEquals(char ch);
  bool ReadField(char termchar, char *field, uint32_t fieldsize);
  bool WriteChar(char ch);
  bool WriteString(const char *str);
  bool WriteStringQuote(const char *str);
  bool ProcessCallCell(void);
  bool ProcessGetCell(void);
  bool ProcessSetCell(void);
  bool ProcessICallCell(void);
  bool ProcessIGetCell(void);
  bool ProcessISetCell(void);
  bool ProcessAddCell(void);
  bool ProcessSubCell(void);
  bool ProcessSaveCell(void);
  bool ProcessCell(void);
  bool ProcessMessage(void);
  void CtlThread(void);

private:
  Device *_lowdev;
  HCContainer *_top;
  uint32_t _readcount;
  char _readbuf[65536];
  uint32_t _readind;
  char _writebuf[65536];
  uint32_t _writeind;
  Thread<HCQServer> *_ctlthread;
};

#endif
