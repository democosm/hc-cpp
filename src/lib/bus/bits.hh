// Bits
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

#ifndef _BITS_HH_
#define _BITS_HH_

#include "bus.hh"
#include <cassert>
#include <inttypes.h>

template <class T>
class Bits
{
public:
  Bits(Bus *bus, uint32_t addr, T mask)
  {
    //Assert valid arguments
    assert((bus != 0) && (mask != 0));

    //Initialize cache variables
    _bus = bus;
    _addr = addr;
    _mask = mask;

    //Determine shift ammount from mask
    for(_shift=0; _shift<sizeof(T)*8; _shift++)
      if(((_mask >> _shift) & (T)1) != 0)
        break;
  }

  virtual ~Bits()
  {
  }

  int Get(T &val)
  {
    //Delegate to bus
    return _bus->Get(_addr, _mask, _shift, val);
  }

  int Set(T val)
  {
    //Delegate to bus
    return _bus->Set(_addr, _mask, _shift, val);
  }

private:
  Bus *_bus;
  uint32_t _addr;
  T _mask;
  uint8_t _shift;
};

//Types derived from template
typedef Bits<uint8_t> Bits8;
typedef Bits<uint16_t> Bits16;
typedef Bits<uint32_t> Bits32;
typedef Bits<uint64_t> Bits64;

#endif //_BITS_HH_
