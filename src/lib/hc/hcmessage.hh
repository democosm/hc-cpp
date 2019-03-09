// HC message
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

#ifndef _HCMESSAGE_HH_
#define _HCMESSAGE_HH_

#include "device.hh"
#include "hccell.hh"
#include <inttypes.h>
#include <string>

class HCMessage
{
public:
  //Message overhead
  static const uint32_t OVERHEAD = 1;

  //Maximum payload size (see cell payload max)
  static const uint32_t PAYLOAD_MAX = 1400;

public:
  HCMessage();
  ~HCMessage();
  void Reset(uint8_t transaction);
  uint8_t GetTransaction(void);
  int Send(Device *dev);
  int Recv(Device *dev);
  bool Read(HCCell *val);
  bool Write(HCCell *val);
  void Print(const std::string &extra);

private:
  uint8_t *_buffer;
  uint8_t *_payload;
  uint32_t _readindex;
  uint32_t _payloadlength;
  uint8_t _transaction;
};

#endif //_HCMESSAGE_HH_
