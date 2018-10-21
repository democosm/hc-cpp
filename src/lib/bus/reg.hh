// Register
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

#ifndef _REG_HH_
#define _REG_HH_

#include "bus.hh"
#include <cassert>
#include <inttypes.h>

template <class T>
class Reg
{
public:
  Reg(Bus *bus, uint32_t addr)
  {
    //Assert valid arguments
    assert(bus != 0);

    //Initialize cache variables
    _bus = bus;
    _addr = addr;
  }

  virtual ~Reg()
  {
  }

  int Get(T &val)
  {
    //Delegate to bus
    return _bus->Get(_addr, val);
  }

  int Set(T val)
  {
    //Delegate to bus
    return _bus->Set(_addr, val);
  }

private:
  Bus *_bus;
  uint32_t _addr;
};

//Types derived from template
typedef Reg<uint8_t> Reg8;
typedef Reg<uint16_t> Reg16;
typedef Reg<uint32_t> Reg32;
typedef Reg<uint64_t> Reg64;

#endif //_REG_HH_
