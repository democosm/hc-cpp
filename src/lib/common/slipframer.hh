// SLIP framer
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

#ifndef _SLIPFRAMER_HH_
#define _SLIPFRAMER_HH_

#include "device.hh"
#include <inttypes.h>

class SLIPFramer : public Device
{
public:
  SLIPFramer(Device *lowdev, uint32_t maxpldsiz);
  virtual ~SLIPFramer();
  virtual uint32_t Read(void *buf, uint32_t maxlen);
  virtual uint32_t Write(void *buf, uint32_t len);

private:
  //Receive states
  static const uint32_t RX_MODE_NORMAL = 0;
  static const uint32_t RX_MODE_ESCAPE = 1;

  //Special bytes
  static const uint8_t BYTE_END = 0xC0;
  static const uint8_t BYTE_ESC = 0xDB;
  static const uint8_t BYTE_ESC_END = 0xDC;
  static const uint8_t BYTE_ESC_ESC = 0xDD;

private:
  Device *_lowdev;
  uint32_t _maxpldsiz;
  uint8_t *_txbuf;
};

#endif //_SLIPFRAMER_HH_
