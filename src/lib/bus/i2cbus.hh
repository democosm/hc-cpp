// I2C bus interface
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

#ifndef _I2CBUS_HH_
#define _I2CBUS_HH_

#include "bus.hh"
#include "i2c.hh"
#include "mutex.hh"
#include <inttypes.h>

class I2CBus : public Bus
{
public:
  I2CBus(I2C *i2c, uint8_t devaddr);
  virtual ~I2CBus();
  virtual int Get(uint32_t addr, uint8_t &val);
  virtual int Get(uint32_t addr, uint8_t mask, uint8_t shift, uint8_t &val);
  virtual int Set(uint32_t addr, uint8_t val);
  virtual int Set(uint32_t addr, uint8_t mask, uint8_t shift, uint8_t val);

private:
  I2C *_i2c;
  uint8_t _devaddr;
  Mutex *_mutex;
};

#endif //_I2CBUS_HH_
