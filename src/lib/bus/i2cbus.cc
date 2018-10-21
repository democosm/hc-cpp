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

#include "error.hh"
#include "i2cbus.hh"

I2CBus::I2CBus(I2C *i2c, uint8_t devaddr)
: Bus()
{
  //Initialize cache variables
  _i2c = i2c;
  _devaddr = devaddr;

  //Create mutex
  _mutex = new Mutex();
}

I2CBus::~I2CBus()
{
  //Delete mutex
  delete _mutex;
}

int I2CBus::Get(uint32_t addr, uint8_t &val)
{
  int terr;

  //Begin mutual exclusion
  _mutex->Wait();

  //Delegate to I2C driver
  terr = _i2c->Get(_devaddr, addr, val);

  //End mutual exclusion
  _mutex->Give();

  return terr;
}

int I2CBus::Get(uint32_t addr, uint8_t mask, uint8_t shift, uint8_t &val)
{
  int terr;
  uint8_t tval;

  //Begin mutual exclusion
  _mutex->Wait();

  //Get value and check for error
  if((terr = _i2c->Get(_devaddr, addr, tval)) != ERR_NONE)
  {
    //End mutual exclusion
    _mutex->Give();

    return terr;
  }

  //Mask, shift and return
  val = (tval & mask) >> shift;

  //End mutual exclusion
  _mutex->Give();

  return ERR_NONE;
}

int I2CBus::Set(uint32_t addr, uint8_t val)
{
  int terr;

  //Begin mutual exclusion
  _mutex->Wait();

  //Delegate to I2C driver
  terr = _i2c->Set(_devaddr, addr, val);

  //End mutual exclusion
  _mutex->Give();

  return terr;
}

int I2CBus::Set(uint32_t addr, uint8_t mask, uint8_t shift, uint8_t val)
{
  int terr;
  uint8_t tval;

  //Begin mutual exclusion
  _mutex->Wait();

  //Get value and check for error
  if((terr = _i2c->Get(_devaddr, addr, tval)) != ERR_NONE)
  {
    //End mutual exclusion
    _mutex->Give();

    return terr;
  }

  //Mask and shift in new value
  tval = (tval & ~mask) | ((val << shift) & mask);

  //Set new value
  terr = _i2c->Set(_devaddr, addr, tval);

  //End mutual exclusion
  _mutex->Give();

  return terr;
}
