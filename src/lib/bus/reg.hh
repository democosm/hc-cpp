// Registers
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

#ifndef _REG_HH_
#define _REG_HH_

#include "bus.hh"
#include <cassert>
#include <inttypes.h>

//Single byte register
class Reg8
{
public:
  Reg8(Bus *bus, uint32_t addr)
  {
    //Assert valid arguments
    assert(bus != 0);

    //Initialize cache variables
    _bus = bus;
    _addr = addr;
  }

  virtual ~Reg8()
  {
  }

  int Get(uint8_t &val)
  {
    int terr;

    //Default value to 0
    val = 0;

    //Reserve bus
    _bus->Reserve();

    //Get data on bus
    terr = _bus->Get(_addr, &val, 1);

    //Release bus
    _bus->Release();

    return terr;
  }

  int Set(uint8_t val)
  {
    int terr;

    //Reserve bus
    _bus->Reserve();

    //Set data on bus
    terr = _bus->Set(_addr, &val, 1);

    //Release bus
    _bus->Release();

    return terr;
  }

private:
  Bus *_bus;
  uint32_t _addr;
};

//Multi-byte network byte order (big endian) register
template <class T>
class BReg
{
public:
  BReg(Bus *bus, uint32_t addr)
  {
    //Assert valid arguments
    assert(bus != 0);

    //Initialize cache variables
    _bus = bus;
    _addr = addr;
  }

  virtual ~BReg()
  {
  }

  int Get(T &val)
  {
    int terr;
    uint8_t buf[sizeof(T)];
    uint32_t i;
    uint32_t j;

    //Reserve bus
    _bus->Reserve();

    //Get data on bus
    terr = _bus->Get(_addr, buf, sizeof(T));

    //Release bus
    _bus->Release();

    //Default value to 0
    val = 0;

    //Check for error
    if(terr != ERR_NONE)
      return terr;

    //Read data from buffer
    for(i=0, j=(sizeof(T)-1)*8; i<sizeof(T); i++, j-=8)
      val |= (T)buf[i] << j;

    return ERR_NONE;
  }

  int Set(T val)
  {
    int terr;
    uint8_t buf[sizeof(T)];
    uint32_t i;
    uint32_t j;

    //Write data to buffer
    for(i=0, j=(sizeof(T)-1)*8; i<sizeof(T); i++, j-=8)
      buf[i] = (uint8_t)(val >> j);

    //Reserve bus
    _bus->Reserve();

    //Set data on bus
    terr = _bus->Set(_addr, buf, sizeof(T));

    //Release bus
    _bus->Release();

    return terr;
  }

private:
  Bus *_bus;
  uint32_t _addr;
};

//Types derived from template
typedef BReg<uint16_t> BReg16;
typedef BReg<uint32_t> BReg32;
typedef BReg<uint64_t> BReg64;

//Multi-byte non-network byte order (little endian) register
template <class T>
class LReg
{
public:
  LReg(Bus *bus, uint32_t addr)
  {
    //Assert valid arguments
    assert(bus != 0);

    //Initialize cache variables
    _bus = bus;
    _addr = addr;
  }

  virtual ~LReg()
  {
  }

  int Get(T &val)
  {
    int terr;
    uint8_t buf[sizeof(T)];
    uint32_t i;

    //Reserve bus
    _bus->Reserve();

    //Get data on bus
    terr = _bus->Get(_addr, buf, sizeof(T));

    //Release bus
    _bus->Release();

    //Default value to 0
    val = 0;

    //Check for error
    if(terr != ERR_NONE)
      return terr;

    //Read data from buffer
    for(i=0; i<sizeof(T); i++)
      val |= (T)buf[i] << (i*8);

    return ERR_NONE;
  }

  int Set(T val)
  {
    int terr;
    uint8_t buf[sizeof(T)];
    uint32_t i;

    //Write data to buffer
    for(i=0; i<sizeof(T); i++)
      buf[i] = (uint8_t)(val >> (i*8));

    //Reserve bus
    _bus->Reserve();

    //Set data on bus
    terr = _bus->Set(_addr, buf, sizeof(T));

    //Release bus
    _bus->Release();

    return terr;
  }

private:
  Bus *_bus;
  uint32_t _addr;
};

//Types derived from template
typedef LReg<uint16_t> LReg16;
typedef LReg<uint32_t> LReg32;
typedef LReg<uint64_t> LReg64;

#endif //_REG_HH_
